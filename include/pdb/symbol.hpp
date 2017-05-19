// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include "address.hpp"
#include "module.hpp"

#include <safeint.h>

#include <Windows.h>
#include <DbgHelp.h>

#include <cstddef>
#include <cstring>

#include <stdexcept>
#include <string>

namespace pdb
{
    class SymbolInfo
    {
    public:
        typedef SYMBOL_INFO Raw;

        SymbolInfo()
            : raw{*reinterpret_cast<Raw*>(buffer)}
        {
            raw.SizeOfStruct = sizeof(Raw);
            raw.MaxNameLen = MAX_SYM_NAME;
        }

        explicit SymbolInfo(const Raw& raw)
            : SymbolInfo{}
        {
            if (raw.SizeOfStruct != sizeof(raw))
                throw std::runtime_error{"unexpected symbol structure size"};
            std::memcpy(buffer, &raw, calc_size(raw));
        }

        explicit operator Raw&() { return raw; }

        explicit operator const Raw&() const { return raw; }

        Address get_displacement() const { return displacement; }

        void set_displacement(Address new_value)
        {
            displacement = new_value;
        }

        std::string get_name() const { return {raw.Name, raw.NameLen}; }

        Address get_offline_base() const { return raw.ModBase; }

        Address get_offline_address() const { return raw.Address; }

        typedef ULONG Tag;

        Tag get_tag() const { return raw.Tag; }

        enum class Type : Tag
        {
            Function = SymTagFunction,
            RESERVED = SymTagMax,
        };

        Type get_type() const { return static_cast<Type>(get_tag()); }

        bool is_function() const { return get_type() == Type::Function; }

    private:
        static std::size_t calc_size(const Raw& raw)
        {
            try
            {
                msl::utilities::SafeInt<std::size_t> size{raw.SizeOfStruct};
                size += raw.NameLen;
                size -= 1;
                return size;
            }
            catch (const msl::utilities::SafeIntException&)
            {
                throw std::runtime_error{"symbol name is too long"};
            }
        }

        unsigned char buffer[sizeof(Raw) + MAX_SYM_NAME - 1] = {0};
        Address displacement = 0;

    protected:
        Raw& raw;
    };

    class Symbol : public SymbolInfo
    {
    public:
        Symbol(Address online_address, const SymbolInfo& info)
            : SymbolInfo{info}
            , online_address{online_address}
        { }

        Address get_online_address() const { return online_address; }

    private:
        const Address online_address;
    };
}
