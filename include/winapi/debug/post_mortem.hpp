// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#pragma once

#include "address.hpp"
#include "dbghelp.hpp"
#include "module.hpp"
#include "symbol.hpp"

#include <winapi/file.hpp>

#include <functional>
#include <map>
#include <string>
#include <unordered_set>

namespace winapi {

class PostMortem {
public:
    PostMortem() = default;

    Address add_pdb(Address online_base, const std::string& path);

    typedef std::function<void(const Symbol&)> OnSymbol;
    void enum_symbols(const OnSymbol&) const;
    void enum_symbols(Address offline_base, const OnSymbol&) const;
    void enum_symbols(const Module&, const OnSymbol&) const;

    Symbol resolve_symbol(Address) const;
    Symbol resolve_symbol(const std::string&) const;

    LineInfo resolve_line(Address) const;

    const Module& module_with_online_base(Address) const;
    const Module& module_with_offline_base(Address) const;

private:
    Symbol symbol_from_buffer(const SymbolInfo&) const;
    static Symbol symbol_from_buffer(const Module&, const SymbolInfo&);

    const Module& module_from_online_address(Address) const;
    const Module& module_from_offline_address(Address) const;

    Address address_offline_to_online(Address) const;
    Address address_online_to_offline(Address) const;

    const DbgHelp dbghelp{DbgHelp::post_mortem()};

    std::unordered_set<winapi::File::ID> file_ids;
    std::map<Address, Module> online_bases;
    std::map<Address, const Module&> offline_bases;
};

} // namespace winapi
