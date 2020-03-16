// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include "address.hpp"
#include "module.hpp"
#include "symbol.hpp"

#include <windows.h>

#include <functional>
#include <string>

namespace pdb {

class DbgHelp {
public:
    static DbgHelp current_process() { return DbgHelp{true}; }
    static DbgHelp post_mortem() { return DbgHelp{false}; }

    void swap(DbgHelp& other) noexcept;

    DbgHelp(DbgHelp&& other) noexcept;
    DbgHelp& operator=(DbgHelp) noexcept;
    ~DbgHelp();

    ModuleInfo load_pdb(const std::string& path) const;

    typedef std::function<void(const ModuleInfo&)> OnModule;
    void enum_modules(const OnModule&) const;

    ModuleInfo resolve_module(Address) const;

    typedef std::function<void(const SymbolInfo&)> OnSymbol;
    static constexpr auto all_symbols = "*!*";
    void enum_symbols(const ModuleInfo&, const std::string& mask, const OnSymbol&) const;
    void enum_symbols(const ModuleInfo&, const OnSymbol&) const;
    void enum_symbols(const std::string& mask, const OnSymbol&) const;
    void enum_symbols(const OnSymbol&) const;

    SymbolInfo resolve_symbol(Address) const;
    SymbolInfo resolve_symbol(const std::string&) const;

    LineInfo resolve_line(Address) const;

private:
    explicit DbgHelp(bool invade_current_process);

    void close();

    HANDLE id = GetCurrentProcess();

    DbgHelp(const DbgHelp&) = delete;
};

inline void swap(DbgHelp& a, DbgHelp& b) noexcept {
    a.swap(b);
}

} // namespace pdb

namespace std {

template <>
inline void swap(pdb::DbgHelp& a, pdb::DbgHelp& b) noexcept {
    a.swap(b);
}

} // namespace std
