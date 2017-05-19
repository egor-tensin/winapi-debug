// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include "pdb/all.hpp"

#include <safeint.h>

#include <cstring>

#include <sstream>
#include <stdexcept>
#include <string>

namespace pdb
{
    ModuleInfo::ModuleInfo()
        : ModuleInfo{create_raw()}
    { }

    ModuleInfo::ModuleInfo(const Raw& raw)
        : raw{raw}
    {
        if (raw.SizeOfStruct != sizeof(raw))
            throw std::runtime_error{"unexpected module structure size"};
    }

    ModuleInfo::Raw ModuleInfo::create_raw()
    {
        Raw raw;
        std::memset(&raw, 0, sizeof(raw));
        raw.SizeOfStruct = sizeof(raw);
        return raw;
    }

    Address Module::translate_offline_address(Address offline) const
    {
        if (offline < get_offline_base())
            throw std::range_error{invalid_offline_address(offline)};
        const auto offset = offline - get_offline_base();
        auto online = offset;
        if (!msl::utilities::SafeAdd(online, get_online_base(), online))
            throw std::range_error{invalid_offline_address(offline)};
        return online;
    }

    Address Module::translate_online_address(Address online) const
    {
        if (online < get_online_base())
            throw std::range_error{invalid_online_address(online)};
        const auto offset = online - get_online_base();
        auto offline = offset;
        if (!msl::utilities::SafeAdd(offline, get_offline_base(), offline))
            throw std::range_error{invalid_online_address(offline)};
        return offline;
    }

    std::string Module::invalid_offline_address(Address offline) const
    {
        std::ostringstream oss;
        oss << "offline address " << format_address(offline)
            << " doesn't belong to module " << get_name()
            << " (base offline address " << format_address(get_offline_base()) << ')';
        return oss.str();
    }

    std::string Module::invalid_online_address(Address online) const
    {
        std::ostringstream oss;
        oss << "online address " << format_address(online)
            << " doesn't belong to module " << get_name()
            << " (base online address " << format_address(get_online_base()) << ')';
        return oss.str();
    }
}
