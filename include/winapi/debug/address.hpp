// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#pragma once

#include <windows.h>

#include <sstream>
#include <string>

namespace winapi {

typedef DWORD64 Address;

namespace address {

inline std::string format(Address address) {
    std::ostringstream oss;
    oss << std::hex << std::showbase << address;
    return oss.str();
}

inline std::string format(void* address) {
    return format(reinterpret_cast<Address>(address));
}

inline bool parse(Address& dest, const std::string& src) {
    std::istringstream iss{src};
    iss >> std::hex;
    char c;
    return iss >> dest && !iss.get(c);
}

} // namespace address
} // namespace winapi
