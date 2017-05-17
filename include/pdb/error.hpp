// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include <Windows.h>

#include <string>
#include <system_error>

namespace pdb
{
    namespace error
    {
        class CategoryWindows : public std::error_category
        {
        public:
            CategoryWindows() = default;

            const char* name() const noexcept { return "Windows"; }

            std::string message(int) const;
        };

        inline const CategoryWindows& category_windows()
        {
            static const CategoryWindows instance;
            return instance;
        }

        inline std::system_error windows(DWORD code)
        {
            static_assert(sizeof(DWORD) == sizeof(int), "Aren't DWORDs the same size as ints?");

            return std::system_error{
                static_cast<int>(code),
                category_windows()};
        }
    }
}
