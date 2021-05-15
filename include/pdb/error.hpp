// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#pragma once

#include <windows.h>

#include <string>
#include <system_error>

namespace pdb {
namespace error {

class CategoryWindows : public std::error_category {
public:
    CategoryWindows() = default;

    const char* name() const noexcept { return "Windows"; }

    std::string message(int) const;
};

inline const CategoryWindows& category_windows() {
    static const CategoryWindows instance;
    return instance;
}

std::system_error windows(DWORD code, const char* function);

} // namespace error
} // namespace pdb
