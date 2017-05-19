// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include "pdb/all.hpp"

#include <map>
#include <stdexcept>
#include <string>
#include <utility>

namespace pdb
{
    namespace
    {
        template <typename Value>
        const Module& guess_module(
            const std::map<Address, Value>& modules,
            Address pivot)
        {
            if (modules.empty())
                throw std::range_error{"there're no modules to choose from"};

            auto it = modules.lower_bound(pivot);

            if (it == modules.cend())
            {
                --it;
                return it->second;
            }

            if (it->first > pivot)
            {
                if (it == modules.cbegin())
                    throw std::range_error{"couldn't choose a module"};
                --it;
                return it->second;
            }

            return it->second;
        }
    }

    Address Repo::add_pdb(Address online_base, const std::string& path)
    {
        Module module{online_base, dbghelp.load_pdb(path)};
        const auto offline_base = module.get_offline_base();
        const auto it = online_modules.emplace(online_base, std::move(module));
        offline_modules.emplace(offline_base, it.first->second);
        return offline_base;
    }

    void Repo::enum_symbols(const OnSymbol& callback) const
    {
        for (const auto& it : offline_modules)
            enum_symbols(it.second, callback);
    }

    void Repo::enum_symbols(Address offline_base, const OnSymbol& callback) const
    {
        const auto it = offline_modules.find(offline_base);
        if (it == offline_modules.cend())
            throw std::runtime_error{"unknown module"};
        enum_symbols(it->second, callback);
    }

    void Repo::enum_symbols(const Module& module, const OnSymbol& callback) const
    {
        dbghelp.enum_symbols(module, [&] (const SymbolInfo& raw)
        {
            callback(symbol_from_buffer(module, raw));
        });
    }

    Symbol Repo::resolve_symbol(Address online) const
    {
        return symbol_from_buffer(dbghelp.resolve_symbol(address_online_to_offline(online)));
    }

    Symbol Repo::resolve_symbol(const std::string& name) const
    {
        return symbol_from_buffer(dbghelp.resolve_symbol(name));
    }

    Symbol Repo::symbol_from_buffer(const SymbolInfo& raw) const
    {
        const auto it = offline_modules.find(raw.get_offline_base());
        if (it == offline_modules.cend())
            throw std::runtime_error{"symbol's module is unknown"};
        return symbol_from_buffer(it->second, raw);
    }

    Symbol Repo::symbol_from_buffer(const Module& module, const SymbolInfo& raw) const
    {
        return {module.translate_offline_address(raw.get_offline_address()), raw};
    }

    Address Repo::address_online_to_offline(Address online) const
    {
        return module_from_online_address(online)
            .translate_online_address(online);
    }

    Address Repo::address_offline_to_online(Address offline) const
    {
        return module_from_offline_address(offline)
            .translate_offline_address(offline);
    }

    const Module& Repo::module_from_online_address(Address online) const
    {
        return guess_module(online_modules, online);
    }

    const Module& Repo::module_from_offline_address(Address offline) const
    {
        return guess_module(offline_modules, offline);
    }
}
