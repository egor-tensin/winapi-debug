// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#pragma once

#include <windows.h>

#include <charconv>
#include <format>
#include <string>
#include <string_view>

namespace winapi {

typedef DWORD64 Address;

namespace address {

inline std::string format(Address address) {
    return std::format("{:#x}", address);
}

inline std::string format(void* address) {
    return format(reinterpret_cast<Address>(address));
}

inline bool parse(Address& dest, std::string_view src) {
    Address result;
    const auto [ptr, ec] = std::from_chars(src.data(), src.data() + src.length(), result, 16);
    if (ec == std::errc()) {
        dest = result;
        return true;
    }
    return false;
}

} // namespace address
} // namespace winapi
