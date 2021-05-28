// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#pragma once

#include "address.hpp"

#include <dbghelp.h>
#include <windows.h>

#include <string>

namespace winapi {

class ModuleInfo {
public:
    typedef IMAGEHLP_MODULEW64 Impl;

    ModuleInfo();
    explicit ModuleInfo(const Impl& impl);

    explicit operator Impl&() { return impl; }
    explicit operator const Impl&() const { return impl; }

    Address get_offline_base() const { return impl.BaseOfImage; }

    std::string get_name() const;

private:
    static Impl create_impl();

    Impl impl;
};

class Module : public ModuleInfo {
public:
    Module(Address online_base, const ModuleInfo& info)
        : ModuleInfo{info}, online_base{online_base} {}

    Address get_online_base() const { return online_base; }

    Address translate_offline_address(Address offline) const;
    Address translate_online_address(Address online) const;

private:
    std::string invalid_offline_address(Address offline) const;
    std::string invalid_online_address(Address online) const;

    const Address online_base;
};

} // namespace winapi
