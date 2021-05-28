// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#include <winapi/debug.hpp>
#include <winapi/file.hpp>

#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace pdb {
namespace {

std::string pdb_already_loaded(Address online_base, const std::string& path) {
    std::ostringstream oss;
    oss << "module with online base address " << format_address(online_base)
        << " has already been loaded: " << path;
    return oss.str();
}

std::string pdb_already_loaded(const std::string& path) {
    std::ostringstream oss;
    oss << "module has already been loaded: " << path;
    return oss.str();
}

std::string offline_base_already_used(Address base) {
    std::ostringstream oss;
    oss << "module with offline base address " << format_address(base)
        << " has already been loaded (shouldn't happen)";
    return oss.str();
}

std::string module_not_found(Address base) {
    std::ostringstream oss;
    oss << "module with base address " << format_address(base) << " wasn't found";
    return oss.str();
}

std::string guess_module_no_modules(Address pivot) {
    std::ostringstream oss;
    oss << "couldn't select a module for address " << format_address(pivot)
        << ": no modules have been loaded yet";
    return oss.str();
}

std::string guess_module_address_too_low(Address pivot) {
    std::ostringstream oss;
    oss << "couldn't select a module for address " << format_address(pivot) << ": it's too low";
    return oss.str();
}

template <typename Value>
const Module& lookup_module(const std::map<Address, Value>& modules, Address base) {
    const auto it = modules.find(base);
    if (it == modules.cend())
        throw std::runtime_error{module_not_found(base)};
    return it->second;
}

template <typename Value>
const Module& guess_module(const std::map<Address, Value>& modules, Address pivot) {
    if (modules.empty())
        throw std::range_error{guess_module_no_modules(pivot)};

    auto it = modules.lower_bound(pivot);

    if (it == modules.cend()) {
        --it;
        return it->second;
    }

    if (it->first > pivot) {
        if (it == modules.cbegin())
            throw std::range_error{guess_module_address_too_low(pivot)};
        --it;
        return it->second;
    }

    return it->second;
}

} // namespace

Address Repo::add_pdb(Address online_base, const std::string& path) {
    if (online_bases.find(online_base) != online_bases.cend())
        throw std::runtime_error{pdb_already_loaded(online_base, path)};

    auto file_id = winapi::File::open_read_attributes(path).query_id();
    if (file_ids.find(file_id) != file_ids.cend())
        throw std::runtime_error{pdb_already_loaded(path)};

    Module module{online_base, dbghelp.load_pdb(path)};
    const auto offline_base = module.get_offline_base();

    if (offline_bases.find(offline_base) != offline_bases.cend())
        throw std::runtime_error{offline_base_already_used(offline_base)};

    file_ids.emplace(std::move(file_id));
    const auto it = online_bases.emplace(online_base, std::move(module));
    offline_bases.emplace(offline_base, it.first->second);

    return offline_base;
}

void Repo::enum_symbols(const OnSymbol& callback) const {
    for (const auto& it : offline_bases)
        enum_symbols(it.second, callback);
}

void Repo::enum_symbols(Address offline_base, const OnSymbol& callback) const {
    const auto it = offline_bases.find(offline_base);
    if (it == offline_bases.cend())
        throw std::runtime_error{"unknown module"};
    enum_symbols(it->second, callback);
}

void Repo::enum_symbols(const Module& module, const OnSymbol& callback) const {
    dbghelp.enum_symbols(
        module, [&](const SymbolInfo& impl) { callback(symbol_from_buffer(module, impl)); });
}

Symbol Repo::resolve_symbol(Address online) const {
    return symbol_from_buffer(dbghelp.resolve_symbol(address_online_to_offline(online)));
}

Symbol Repo::resolve_symbol(const std::string& name) const {
    return symbol_from_buffer(dbghelp.resolve_symbol(name));
}

LineInfo Repo::resolve_line(Address online) const {
    return dbghelp.resolve_line(address_online_to_offline(online));
}

const Module& Repo::module_with_online_base(Address base) const {
    return lookup_module(online_bases, base);
}

const Module& Repo::module_with_offline_base(Address base) const {
    return lookup_module(offline_bases, base);
}

Symbol Repo::symbol_from_buffer(const SymbolInfo& impl) const {
    return symbol_from_buffer(module_with_offline_base(impl.get_offline_base()), impl);
}

Symbol Repo::symbol_from_buffer(const Module& module, const SymbolInfo& impl) {
    return {module.translate_offline_address(impl.get_offline_address()), impl};
}

Address Repo::address_online_to_offline(Address online) const {
    return module_from_online_address(online).translate_online_address(online);
}

Address Repo::address_offline_to_online(Address offline) const {
    return module_from_offline_address(offline).translate_offline_address(offline);
}

const Module& Repo::module_from_online_address(Address online) const {
    return guess_module(online_bases, online);
}

const Module& Repo::module_from_offline_address(Address offline) const {
    return guess_module(offline_bases, offline);
}

} // namespace pdb
