// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include "pdb/all.hpp"

#include <SafeInt.hpp>

#include <dbghelp.h>
#include <windows.h>

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace pdb {
namespace {

void set_dbghelp_options() {
    SymSetOptions(SymGetOptions() | SYMOPT_DEBUG | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
}

void initialize(HANDLE id, bool invade_current_process) {
    set_dbghelp_options();

    if (!SymInitialize(id, NULL, invade_current_process ? TRUE : FALSE))
        throw error::windows(GetLastError());
}

void clean_up(HANDLE id) {
    if (!SymCleanup(id))
        throw error::windows(GetLastError());
}

Address next_offline_base = 0x10000000;

Address gen_next_offline_base(std::size_t pdb_size) {
    const auto base = next_offline_base;
    if (!SafeAdd(next_offline_base, pdb_size, next_offline_base))
        throw std::runtime_error{
            "no more PDB files can be added, the internal address space is exhausted"};
    return base;
}

ModuleInfo get_module_info(HANDLE id, Address offline_base) {
    ModuleInfo info;

    if (!SymGetModuleInfo64(id, offline_base, &static_cast<ModuleInfo::Impl&>(info)))
        throw error::windows(GetLastError());

    return info;
}

struct ModuleEnumerator {
    HANDLE id;
    DbgHelp::OnModule callback;
};

BOOL CALLBACK enum_modules_callback(PCSTR, DWORD64 offline_base, PVOID raw_enumerator_ptr) {
    const auto enumerator_ptr = reinterpret_cast<ModuleEnumerator*>(raw_enumerator_ptr);
    const auto& enumerator = *enumerator_ptr;
    enumerator.callback(get_module_info(enumerator.id, offline_base));
    return TRUE;
}

BOOL CALLBACK enum_symbols_callback(SYMBOL_INFO* info, ULONG, VOID* raw_callback_ptr) {
    const auto callback_ptr = reinterpret_cast<DbgHelp::OnSymbol*>(raw_callback_ptr);
    const auto& callback = *callback_ptr;
    callback(SymbolInfo{*info});
    return TRUE;
}

void enum_symbols(HANDLE id,
                  Address module_base,
                  const std::string& mask,
                  const DbgHelp::OnSymbol& callback) {
    if (!SymEnumSymbols(id,
                        module_base,
                        mask.c_str(),
                        &enum_symbols_callback,
                        const_cast<DbgHelp::OnSymbol*>(&callback)))
        throw error::windows(GetLastError());
}

} // namespace

DbgHelp::DbgHelp(bool invade_current_process) {
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
    clean_up(id);
}

ModuleInfo DbgHelp::load_pdb(const std::string& path) const {
    DWORD size = 0;

    if (!SafeCast(file::get_size(path), size))
        throw std::range_error{"PDB file is too large"};

    // MinGW-w64 (as of version 7.0) requires PSTR as the third argument.
    std::vector<char> _path;
    _path.reserve(path.length() + 1);
    _path.assign(path.cbegin(), path.cend());
    _path.emplace_back('\0');

    const auto offline_base =
        SymLoadModule64(id, NULL, _path.data(), NULL, gen_next_offline_base(size), size);

    if (!offline_base)
        throw error::windows(GetLastError());

    return get_module_info(id, offline_base);
}

void DbgHelp::enum_modules(const OnModule& callback) const {
    ModuleEnumerator enumerator{id, callback};
    if (!SymEnumerateModules64(id, &enum_modules_callback, &enumerator))
        throw error::windows(GetLastError());
}

ModuleInfo DbgHelp::resolve_module(Address offline) const {
    return get_module_info(id, offline);
}

void DbgHelp::enum_symbols(const ModuleInfo& module,
                           const std::string& mask,
                           const OnSymbol& callback) const {
    pdb::enum_symbols(id, module.get_offline_base(), mask, callback);
}

void DbgHelp::enum_symbols(const ModuleInfo& module, const OnSymbol& callback) const {
    enum_symbols(module, all_symbols, callback);
}

void DbgHelp::enum_symbols(const std::string& mask, const OnSymbol& callback) const {
    pdb::enum_symbols(id, 0, mask, callback);
}

void DbgHelp::enum_symbols(const OnSymbol& callback) const {
    enum_symbols(all_symbols, callback);
}

SymbolInfo DbgHelp::resolve_symbol(Address offline) const {
    Address displacement = 0;
    SymbolInfo symbol;

    if (!SymFromAddr(id, offline, &displacement, &static_cast<SYMBOL_INFO&>(symbol)))
        throw error::windows(GetLastError());

    symbol.set_displacement(displacement);
    return symbol;
}

SymbolInfo DbgHelp::resolve_symbol(const std::string& name) const {
    SymbolInfo symbol;

    if (!SymFromName(id, name.c_str(), &static_cast<SYMBOL_INFO&>(symbol)))
        throw error::windows(GetLastError());

    return symbol;
}

LineInfo DbgHelp::resolve_line(Address offline) const {
    IMAGEHLP_LINE64 impl;
    std::memset(&impl, 0, sizeof(impl));
    impl.SizeOfStruct = sizeof(impl);

    DWORD displacement = 0;

    if (!SymGetLineFromAddr64(id, offline, &displacement, &impl))
        throw error::windows(GetLastError());

    return LineInfo{impl};
}

} // namespace pdb
