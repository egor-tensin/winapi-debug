// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include "address.hpp"

#include <Windows.h>
#include <DbgHelp.h>

#include <cstring>

#include <sstream>
#include <stdexcept>
#include <string>

namespace pdb
{
    class ModuleInfo
    {
    public:
        typedef IMAGEHLP_MODULE64 Raw;

        ModuleInfo()
            : raw{prepare_buffer()}
        { }

        explicit ModuleInfo(const Raw& raw)
            : raw{raw}
        { }

        explicit operator Raw&() { return raw; }

        explicit operator const Raw&() const { return raw; }

        Address get_offline_base() const { return raw.BaseOfImage; }

        std::string get_name() const { return raw.ModuleName; }

    private:
        static Raw prepare_buffer()
        {
            Raw raw;
            std::memset(&raw, 0, sizeof(raw));
            raw.SizeOfStruct = sizeof(raw);
            return raw;
        }

        Raw raw;
    };

    class Module : public ModuleInfo
    {
    public:
        Module(Address online_base, const ModuleInfo& info)
            : ModuleInfo{info}
            , online_base{online_base}
        { }

        Address get_online_base() const { return online_base; }

        Address translate_offline_address(Address offline) const
        {
            if (offline < get_offline_base())
                throw std::range_error{invalid_offline_address(offline)};
            return offline - get_offline_base() + get_online_base();
        }

        Address translate_online_address(Address online) const
        {
            if (online < get_online_base())
                throw std::range_error{invalid_online_address(online)};
            return online - get_online_base() + get_offline_base();
        }

    private:
        std::string invalid_offline_address(Address offline) const
        {
            std::ostringstream oss;
            oss << "offline address " << format_address(offline)
                << " doesn't belong to module " << get_name()
                << " (base offline address " << format_address(get_offline_base()) << ')';
            return oss.str();
        }

        std::string invalid_online_address(Address online) const
        {
            std::ostringstream oss;
            oss << "online address " << format_address(online)
                << " doesn't belong to module " << get_name()
                << " (base online address " << format_address(get_online_base()) << ')';
            return oss.str();
        }

        const Address online_base;
    };
}
