// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#include <winapi/debug.hpp>
#include <winapi/utf8.hpp>

#include <cstring>
#include <format>
#include <limits>
#include <stdexcept>
#include <string>

namespace winapi {

ModuleInfo::ModuleInfo() : ModuleInfo{create_impl()} {}

ModuleInfo::ModuleInfo(const Impl& impl) : impl{impl} {
    if (impl.SizeOfStruct != sizeof(impl))
        throw std::runtime_error{"invalid IMAGEHLP_MODULE64.SizeOfStruct"};
}

ModuleInfo::Impl ModuleInfo::create_impl() {
    Impl impl;
    std::memset(&impl, 0, sizeof(impl));
    impl.SizeOfStruct = sizeof(impl);
    return impl;
}

std::string ModuleInfo::get_name() const {
    return winapi::narrow(impl.ModuleName);
}

Address Module::translate_offline_address(Address offline) const {
    if (offline < get_offline_base())
        throw std::range_error{invalid_offline_address(offline)};
    const auto offset = offline - get_offline_base();
    auto online = offset;
    // Check that it fits the address space.
    const auto max_addr = std::numeric_limits<decltype(online)>::max();
    if (online > max_addr - get_online_base())
        throw std::range_error{invalid_offline_address(offline)};
    online += get_online_base();
    return online;
}

Address Module::translate_online_address(Address online) const {
    if (online < get_online_base())
        throw std::range_error{invalid_online_address(online)};
    const auto offset = online - get_online_base();
    auto offline = offset;
    // Check that it fits the address space.
    const auto max_addr = std::numeric_limits<decltype(offline)>::max();
    if (offline > max_addr - get_offline_base())
        throw std::range_error{invalid_online_address(offline)};
    offline += get_offline_base();
    return offline;
}

std::string Module::invalid_offline_address(Address offline) const {
    return std::format(
        "offline address {} doesn't belong to module {} (base offline address {})",
        address::format(offline),
        get_name(),
        address::format(get_offline_base())
    );
}

std::string Module::invalid_online_address(Address online) const {
    return std::format(
        "online address {} doesn't belong to module {} (base online address {})",
        address::format(online),
        get_name(),
        address::format(get_online_base())
    );
}

} // namespace winapi
