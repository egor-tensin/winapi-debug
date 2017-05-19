// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include "address.hpp"
#include "dbghelp.hpp"
#include "module.hpp"
#include "symbol.hpp"
#include "utils/file.hpp"

#include <functional>
#include <map>
#include <string>
#include <unordered_set>

namespace pdb
{
    class Repo
    {
    public:
        Repo() = default;

        Address add_pdb(Address online_base, const std::string& path);

        typedef std::function<void (const Symbol&)> OnSymbol;
        void enum_symbols(const OnSymbol&) const;
        void enum_symbols(Address offline_base, const OnSymbol&) const;
        void enum_symbols(const Module&, const OnSymbol&) const;

        Symbol resolve_symbol(Address) const;
        Symbol resolve_symbol(const std::string&) const;

    private:
        Symbol symbol_from_buffer(const SymbolInfo&) const;
        Symbol symbol_from_buffer(const Module&, const SymbolInfo&) const;

        const Module& module_from_online_address(Address) const;
        const Module& module_from_offline_address(Address) const;

        Address address_offline_to_online(Address) const;
        Address address_online_to_offline(Address) const;

        const DbgHelp dbghelp;

        std::map<Address, Module> online_modules;
        std::map<Address, const Module&> offline_modules;

        std::unordered_set<file::ID> module_ids;
    };
}
