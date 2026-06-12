// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#pragma once

#include "address.hpp"
#include "module.hpp"
#include "symbol.hpp"

#include <windows.h>

#include <functional>
#include <string_view>

namespace winapi {

class DbgHelp {
public:
    static DbgHelp current_process() {
        return DbgHelp{true};
    }

    static DbgHelp post_mortem() {
        return DbgHelp{false};
    }

    void swap(DbgHelp& other) noexcept;

    DbgHelp(DbgHelp&& other) noexcept;
    DbgHelp& operator=(DbgHelp) noexcept;
    ~DbgHelp();

    ModuleInfo load_pdb(std::string_view path) const;

    using OnModule = std::function<void(const ModuleInfo&)>;
    void enum_modules(const OnModule&) const;

    ModuleInfo resolve_module(Address) const;

    using OnSymbol = std::function<void(const SymbolInfo&)>;
    static constexpr auto all_symbols = "*!*";
    void enum_symbols(const ModuleInfo&, std::string_view mask, const OnSymbol&) const;
    void enum_symbols(const ModuleInfo&, const OnSymbol&) const;
    void enum_symbols(std::string_view mask, const OnSymbol&) const;
    void enum_symbols(const OnSymbol&) const;

    SymbolInfo resolve_symbol(Address) const;
    SymbolInfo resolve_symbol(std::string_view) const;

    LineInfo resolve_line(Address) const;

private:
    explicit DbgHelp(bool invade_current_process);

    void close();

    HANDLE id = NULL;

    DbgHelp(const DbgHelp&) = delete;
};

inline void swap(DbgHelp& a, DbgHelp& b) noexcept {
    a.swap(b);
}

} // namespace winapi

namespace std {

template <>
inline void swap(winapi::DbgHelp& a, winapi::DbgHelp& b) noexcept {
    a.swap(b);
}

} // namespace std
