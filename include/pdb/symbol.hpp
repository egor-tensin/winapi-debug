// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include "address.hpp"
#include "module.hpp"

#include <SafeInt.hpp>

#include <Windows.h>
#pragma warning(push, 0)
#include <DbgHelp.h>
#pragma warning(pop)

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>

namespace pdb {

class SymbolInfo {
public:
    typedef SYMBOL_INFO Raw;

    SymbolInfo() : raw{*reinterpret_cast<Raw*>(buffer)} {
        raw.SizeOfStruct = sizeof(Raw);
        raw.MaxNameLen = MAX_SYM_NAME;
    }

    explicit SymbolInfo(const Raw& raw) : SymbolInfo{} {
        if (raw.SizeOfStruct != sizeof(raw))
            throw std::runtime_error{"invalid SYMBOL_INFO.SizeOfStruct"};
        const auto raw_size = calc_size(raw);
        if (raw_size > sizeof(buffer))
            throw std::runtime_error{"SYMBOL_INFO is too large"};
        std::memcpy(buffer, &raw, raw_size);
    }

    explicit operator Raw&() { return raw; }

    explicit operator const Raw&() const { return raw; }

    Address get_displacement() const { return displacement; }

    void set_displacement(Address new_value) { displacement = new_value; }

    std::string get_name() const { return {raw.Name, raw.NameLen}; }

    Address get_offline_base() const { return raw.ModBase; }

    Address get_offline_address() const { return raw.Address; }

    typedef ULONG Tag;

    Tag get_tag() const { return raw.Tag; }

    enum class Type : Tag {
        Function = SymTagFunction,
        RESERVED = SymTagMax,
    };

    Type get_type() const { return static_cast<Type>(get_tag()); }

    bool is_function() const { return get_type() == Type::Function; }

private:
    static constexpr std::size_t max_buffer_size = sizeof(Raw) + MAX_SYM_NAME - 1;

    static std::size_t calc_size(const Raw& raw) {
        try {
            return SafeInt<std::size_t>{raw.SizeOfStruct} + raw.NameLen - 1;
        } catch (const SafeIntException&) {
            throw std::runtime_error{"invalid SYMBOL_INFO size"};
        }
    }

    unsigned char buffer[max_buffer_size] = {0};
    Address displacement = 0;

protected:
    Raw& raw;
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
    typedef IMAGEHLP_LINE64 Raw;

    explicit LineInfo(const Raw& raw)
        : file_path{raw.FileName}, line_number{cast_line_number(raw.LineNumber)} {}

    const std::string file_path;
    const unsigned long line_number;

private:
    static unsigned long cast_line_number(DWORD raw) {
        unsigned long dest = 0;

        if (!SafeCast(raw, dest))
            throw std::runtime_error{"invalid line number"};

        return dest;
    }
};

} // namespace pdb
