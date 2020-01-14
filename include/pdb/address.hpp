// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include <Windows.h>

#include <sstream>
#include <string>

namespace pdb {

typedef DWORD64 Address;

inline std::string format_address(Address address) {
    std::ostringstream oss;
    oss << std::hex << std::showbase << address;
    return oss.str();
}

inline bool parse_address(Address& dest, const std::string& src) {
    std::istringstream iss{src};
    iss >> std::hex;
    char c;
    return iss >> dest && !iss.get(c);
}

} // namespace pdb
