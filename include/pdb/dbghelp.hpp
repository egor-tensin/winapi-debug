// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include "address.hpp"
#include "module.hpp"
#include "symbol.hpp"

#include <Windows.h>

#include <functional>
#include <string>

namespace pdb {

class DbgHelp {
public:
    DbgHelp(bool invade_current_process = false);
    ~DbgHelp();

    void close();

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
    const HANDLE id = GetCurrentProcess();
    bool closed = false;

    DbgHelp(const DbgHelp&) = delete;
    DbgHelp& operator=(const DbgHelp&) = delete;
};

} // namespace pdb
