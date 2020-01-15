// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include "address.hpp"
#include "module.hpp"

#include <SafeInt.hpp>

#include <DbgHelp.h>
#include <Windows.h>

#include <climits>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>

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
    typedef SYMBOL_INFO Impl;

    SymbolInfo() : impl{*reinterpret_cast<Impl*>(buffer)} {
        impl.SizeOfStruct = sizeof(Impl);
        impl.MaxNameLen = MAX_SYM_NAME;
    }

    explicit SymbolInfo(const Impl& impl) : SymbolInfo{} {
        if (impl.SizeOfStruct != sizeof(impl))
            throw std::runtime_error{"invalid SYMBOL_INFO.SizeOfStruct"};
        const auto raw_size = calc_size(impl);
        if (raw_size > sizeof(buffer))
            throw std::runtime_error{"SYMBOL_INFO is too large"};
        std::memcpy(buffer, &impl, raw_size);
    }

    explicit operator Impl&() { return impl; }

    explicit operator const Impl&() const { return impl; }

    Address get_displacement() const { return displacement; }

    void set_displacement(Address new_value) { displacement = new_value; }

    std::string get_name() const { return {impl.Name, impl.NameLen}; }

    Address get_offline_base() const { return impl.ModBase; }

    Address get_offline_address() const { return impl.Address; }

    symbol::Tag get_tag() const { return impl.Tag; }

    enum class Type : symbol::Tag {
        Function = symbol::SYM_TAG_FUNCTION,
        RESERVED = ULONG_MAX,
    };

    Type get_type() const { return static_cast<Type>(get_tag()); }

    bool is_function() const { return get_type() == Type::Function; }

private:
    static constexpr std::size_t max_buffer_size = sizeof(Impl) + MAX_SYM_NAME - 1;

    static std::size_t calc_size(const Impl& impl) {
        try {
            return SafeInt<std::size_t>{impl.SizeOfStruct} + impl.NameLen - 1;
        } catch (const SafeIntException&) {
            throw std::runtime_error{"invalid SYMBOL_INFO size"};
        }
    }

    unsigned char buffer[max_buffer_size] = {0};
    Address displacement = 0;

protected:
    Impl& impl;
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
    typedef IMAGEHLP_LINE64 Impl;

    explicit LineInfo(const Impl& impl)
        : file_path{impl.FileName}, line_number{cast_line_number(impl.LineNumber)} {}

    const std::string file_path;
    const unsigned long line_number;

private:
    static unsigned long cast_line_number(DWORD impl) {
        unsigned long dest = 0;

        if (!SafeCast(impl, dest))
            throw std::runtime_error{"invalid line number"};

        return dest;
    }
};

} // namespace pdb
