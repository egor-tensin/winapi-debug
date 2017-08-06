// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include "pdb/all.hpp"

#include <safeint.h>

#include <Windows.h>

#include <cstddef>

#include <stdexcept>
#include <string>

namespace pdb
{
    namespace file
    {
        std::size_t get_size(const std::string& path)
        {
            const Handle handle{CreateFileA(
                path.c_str(),
                FILE_READ_ATTRIBUTES,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL)};

            if (handle.get() == INVALID_HANDLE_VALUE)
                throw error::windows(GetLastError());

            LARGE_INTEGER size;

            if (!GetFileSizeEx(handle.get(), &size))
                throw error::windows(GetLastError());

            std::size_t result = 0;

            if (!msl::utilities::SafeCast(size.QuadPart, result))
                throw std::runtime_error{"invalid file size"};

            return result;
        }

        ID query_id(const std::string& path)
        {
            const Handle handle{CreateFileA(
                path.c_str(),
                FILE_READ_ATTRIBUTES,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL)};

            if (handle.get() == INVALID_HANDLE_VALUE)
                throw error::windows(GetLastError());

            FILE_ID_INFO id;

            if (!GetFileInformationByHandleEx(
                    handle.get(),
                    FileIdInfo,
                    &id,
                    sizeof(id)))
                throw error::windows(GetLastError());

            return {id};
        }
    }
}
