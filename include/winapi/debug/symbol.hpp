// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#pragma once

#include "address.hpp"
#include "module.hpp"

#include <dbghelp.h>
#include <windows.h>

#include <array>
#include <climits>
#include <cstddef>
#include <string>
#include <type_traits>

namespace pdb {
namespace symbol {

// MinGW-w64 (as of version 7.0) doesn't have SymTagEnum
typedef ULONG Tag;

constexpr Tag SYM_TAG_FUNCTION = 5;

#ifdef _MSC_VER
static_assert(static_cast<Tag>(SymTagFunction) == SYM_TAG_FUNCTION,
              "unexpected SymTagFunction value");
#endif

} // namespace symbol

class SymbolInfo {
public:
    typedef SYMBOL_INFOW Impl;

    SymbolInfo();
    explicit SymbolInfo(const Impl& impl);

    explicit operator Impl&() { return get_impl(); }
    explicit operator const Impl&() const { return get_impl(); }

    Address get_displacement() const { return displacement; }
    void set_displacement(Address new_value) { displacement = new_value; }

    std::string get_name() const;

    Address get_offline_base() const { return get_impl().ModBase; }
    Address get_offline_address() const { return get_impl().Address; }

    symbol::Tag get_tag() const { return get_impl().Tag; }

    enum class Type : symbol::Tag {
        Function = symbol::SYM_TAG_FUNCTION,
        RESERVED = ULONG_MAX,
    };

    Type get_type() const { return static_cast<Type>(get_tag()); }

    bool is_function() const { return get_type() == Type::Function; }

private:
    static constexpr std::size_t char_size = sizeof(std::remove_extent<decltype(Impl::Name)>::type);
    static_assert(char_size == sizeof(wchar_t), "Aren't we using the wide WinAPI?");
    static constexpr std::size_t max_buffer_size = sizeof(Impl) + (MAX_SYM_NAME - 1) * char_size;

    std::array<unsigned char, max_buffer_size> buffer;
    Address displacement = 0;

    const Impl& get_impl() const { return *reinterpret_cast<const Impl*>(buffer.data()); }
    Impl& get_impl() { return *reinterpret_cast<Impl*>(buffer.data()); }
};

class Symbol : public SymbolInfo {
public:
    Symbol(Address online_address, const SymbolInfo& info)
        : SymbolInfo{info}, online_address{online_address} {}

    Address get_online_address() const { return online_address; }

private:
    const Address online_address;
};

class LineInfo {
public:
    typedef IMAGEHLP_LINEW64 Impl;

    explicit LineInfo(const Impl& impl);

    const std::string file_path;
    const unsigned long line_number;
};

} // namespace pdb
