// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#include <pdb/all.hpp>

#include <winapi/utf8.hpp>

#include <windows.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace pdb {
namespace file {

std::size_t get_size(const std::string& path) {
    const Handle handle{CreateFileW(winapi::widen(path).c_str(),
                                    FILE_READ_ATTRIBUTES,
                                    FILE_SHARE_READ,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL)};

    if (handle.get() == INVALID_HANDLE_VALUE)
        throw error::windows(GetLastError(), "CreateFileW");

    LARGE_INTEGER size;

    if (!GetFileSizeEx(handle.get(), &size))
        throw error::windows(GetLastError(), "GetFileSizeEx");

    if (size.QuadPart < 0 || size.QuadPart > SIZE_MAX)
        throw std::runtime_error{"invalid file size"};
    return static_cast<std::size_t>(size.QuadPart);
}

ID query_id(const std::string& path) {
    const Handle handle{CreateFileW(winapi::widen(path).c_str(),
                                    FILE_READ_ATTRIBUTES,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL)};

    if (handle.get() == INVALID_HANDLE_VALUE)
        throw error::windows(GetLastError(), "CreateFileW");

    FILE_ID_INFO id;

    if (!GetFileInformationByHandleEx(handle.get(), FileIdInfo, &id, sizeof(id)))
        throw error::windows(GetLastError(), "GetFileInformationByHandleEx");

    return {id};
}

} // namespace file
} // namespace pdb
