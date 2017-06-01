// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include "address.hpp"

#include <Windows.h>
#pragma warning(push, 0)
#include <DbgHelp.h>
#pragma warning(pop)

#include <string>

namespace pdb
{
    class ModuleInfo
    {
    public:
        typedef IMAGEHLP_MODULE64 Raw;

        ModuleInfo();
        explicit ModuleInfo(const Raw& raw);

        explicit operator Raw&() { return raw; }
        explicit operator const Raw&() const { return raw; }

        Address get_offline_base() const { return raw.BaseOfImage; }

        std::string get_name() const { return raw.ModuleName; }

    private:
        static Raw create_raw();

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

        Address translate_offline_address(Address offline) const;
        Address translate_online_address(Address online) const;

    private:
        std::string invalid_offline_address(Address offline) const;
        std::string invalid_online_address(Address online) const;

        const Address online_base;
    };
}
