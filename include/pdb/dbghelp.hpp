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

namespace pdb
{
    class DbgHelp
    {
    public:
        DbgHelp();
        ~DbgHelp();

        ModuleInfo load_pdb(const std::string& path) const;

        typedef std::function<void (const SymbolInfo&)> OnSymbol;
        void enum_symbols(const ModuleInfo&, const OnSymbol&) const;

        SymbolInfo resolve_symbol(Address) const;
        SymbolInfo resolve_symbol(const std::string&) const;

        void close();

    private:
        ModuleInfo get_module_info(Address offline_base) const;

        const HANDLE id = GetCurrentProcess();
        bool closed = false;

        DbgHelp(const DbgHelp&) = delete;
        DbgHelp& operator=(const DbgHelp&) = delete;
    };
}
