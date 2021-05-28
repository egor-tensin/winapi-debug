// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#include <winapi/debug.hpp>
#include <winapi/error.hpp>
#include <winapi/file.hpp>
#include <winapi/utf8.hpp>

#include <dbghelp.h>
#include <windows.h>

#include <cstddef>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace winapi {
namespace {

void set_dbghelp_options() {
    SymSetOptions(SymGetOptions() | SYMOPT_DEBUG | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
}

void initialize(HANDLE id, bool invade_current_process) {
    set_dbghelp_options();

    if (!SymInitialize(id, NULL, invade_current_process ? TRUE : FALSE))
        throw winapi::error::windows(GetLastError(), "SymInitialize");
}

void clean_up(HANDLE id) {
    if (!SymCleanup(id))
        throw winapi::error::windows(GetLastError(), "SymCleanup");
}

Address next_offline_base = 0x10000000;

Address gen_next_offline_base(std::size_t pdb_size) {
    const auto base = next_offline_base;

    const auto max_addr = std::numeric_limits<decltype(next_offline_base)>::max();
    if (max_addr - next_offline_base < pdb_size)
        throw std::runtime_error{
            "no more PDB files can be added, the internal address space is exhausted"};
    next_offline_base += pdb_size;

    return base;
}

ModuleInfo get_module_info(HANDLE id, Address offline_base) {
    ModuleInfo info;

    if (!SymGetModuleInfoW64(id, offline_base, &static_cast<ModuleInfo::Impl&>(info)))
        throw winapi::error::windows(GetLastError(), "SymGetModuleInfoW64");

    return info;
}

struct ModuleEnumerator {
    HANDLE id;
    DbgHelp::OnModule callback;
};

BOOL CALLBACK enum_modules_callback(PCWSTR, DWORD64 offline_base, PVOID raw_enumerator_ptr) {
    const auto enumerator_ptr = reinterpret_cast<ModuleEnumerator*>(raw_enumerator_ptr);
    const auto& enumerator = *enumerator_ptr;
    enumerator.callback(get_module_info(enumerator.id, offline_base));
    return TRUE;
}

BOOL CALLBACK enum_symbols_callback(SYMBOL_INFOW* info, ULONG, VOID* raw_callback_ptr) {
    const auto callback_ptr = reinterpret_cast<DbgHelp::OnSymbol*>(raw_callback_ptr);
    const auto& callback = *callback_ptr;
    callback(SymbolInfo{*info});
    return TRUE;
}

void enum_symbols(HANDLE id,
                  Address module_base,
                  const std::string& mask,
                  const DbgHelp::OnSymbol& callback) {
    if (!SymEnumSymbolsW(id,
                         module_base,
                         winapi::widen(mask).c_str(),
                         &enum_symbols_callback,
                         const_cast<DbgHelp::OnSymbol*>(&callback)))
        throw winapi::error::windows(GetLastError(), "SymEnumSymbolsW");
}

} // namespace

DbgHelp::DbgHelp(bool invade_current_process) : id{GetCurrentProcess()} {
    initialize(id, invade_current_process);
}

void DbgHelp::swap(DbgHelp& other) noexcept {
    using std::swap;
    swap(id, other.id);
}

DbgHelp::DbgHelp(DbgHelp&& other) noexcept {
    swap(other);
}

DbgHelp& DbgHelp::operator=(DbgHelp other) noexcept {
    swap(other);
    return *this;
}

DbgHelp::~DbgHelp() {
    try {
        close();
    } catch (...) {
    }
}

void DbgHelp::close() {
    if (id != NULL)
        clean_up(id);
}

ModuleInfo DbgHelp::load_pdb(const std::string& path) const {
    DWORD size = 0;

    {
        const auto raw_size = winapi::File::open_read_attributes(path).get_size();
        if (raw_size > std::numeric_limits<decltype(size)>::max())
            throw std::range_error{"PDB file is too large"};
        size = static_cast<decltype(size)>(raw_size);
    }

    // MinGW-w64 (as of version 7.0) requires PSTR as the third argument.
    std::vector<char> _path;
    _path.reserve(path.length() + 1);
    _path.assign(path.cbegin(), path.cend());
    _path.emplace_back('\0');

    // TODO: switch to the W version?
    const auto offline_base =
        SymLoadModule64(id, NULL, _path.data(), NULL, gen_next_offline_base(size), size);

    if (!offline_base)
        throw winapi::error::windows(GetLastError(), "SymLoadModule64");

    return get_module_info(id, offline_base);
}

void DbgHelp::enum_modules(const OnModule& callback) const {
    ModuleEnumerator enumerator{id, callback};
    if (!SymEnumerateModulesW64(id, &enum_modules_callback, &enumerator))
        throw winapi::error::windows(GetLastError(), "SymEnumerateModulesW64");
}

ModuleInfo DbgHelp::resolve_module(Address offline) const {
    return get_module_info(id, offline);
}

void DbgHelp::enum_symbols(const ModuleInfo& module,
                           const std::string& mask,
                           const OnSymbol& callback) const {
    winapi::enum_symbols(id, module.get_offline_base(), mask, callback);
}

void DbgHelp::enum_symbols(const ModuleInfo& module, const OnSymbol& callback) const {
    enum_symbols(module, all_symbols, callback);
}

void DbgHelp::enum_symbols(const std::string& mask, const OnSymbol& callback) const {
    winapi::enum_symbols(id, 0, mask, callback);
}

void DbgHelp::enum_symbols(const OnSymbol& callback) const {
    enum_symbols(all_symbols, callback);
}

SymbolInfo DbgHelp::resolve_symbol(Address offline) const {
    Address displacement = 0;
    SymbolInfo symbol;

    if (!SymFromAddrW(id, offline, &displacement, &static_cast<SYMBOL_INFOW&>(symbol)))
        throw winapi::error::windows(GetLastError(), "SymFromAddrW");

    symbol.set_displacement(displacement);
    return symbol;
}

SymbolInfo DbgHelp::resolve_symbol(const std::string& name) const {
    SymbolInfo symbol;

    if (!SymFromNameW(id, winapi::widen(name).c_str(), &static_cast<SYMBOL_INFOW&>(symbol)))
        throw winapi::error::windows(GetLastError(), "SymFromNameW");

    return symbol;
}

LineInfo DbgHelp::resolve_line(Address offline) const {
    IMAGEHLP_LINEW64 impl;
    std::memset(&impl, 0, sizeof(impl));
    impl.SizeOfStruct = sizeof(impl);

    DWORD displacement = 0;

    if (!SymGetLineFromAddrW64(id, offline, &displacement, &impl))
        throw winapi::error::windows(GetLastError(), "SymGetLineFromAddrW64");

    return LineInfo{impl};
}

} // namespace winapi
