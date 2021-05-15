// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#include <pdb/all.hpp>

#include <winapi/utf8.hpp>

#include <windows.h>

#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace pdb {
namespace {

// Permissions required for MiniDumpWriteDump.
constexpr DWORD permissions = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ;

Handle open_process(DWORD id) {
    Handle process{OpenProcess(permissions, FALSE, id)};
    if (!process) {
        throw error::windows(GetLastError(), "OpenProcess");
    }
    return process;
}

class PathBuffer {
public:
    PathBuffer() : size{min_size} { data.resize(size); }

    DWORD get_size() const { return size; }

    wchar_t* get_data() { return data.data(); }

    void grow() {
        if (size < min_size) {
            size = min_size;
        } else {
            // Check if we can still multiply by two.
            if (std::numeric_limits<decltype(size)>::max() - size < size)
                throw std::range_error{"couldn't allocate buffer sufficient for a file path"};
            size *= 2;
        }
        data.resize(size);
    }

private:
    static constexpr DWORD min_size = 256;

    DWORD size;
    std::vector<wchar_t> data;
};

std::string get_current_executable_path(PathBuffer& buffer) {
    SetLastError(ERROR_SUCCESS);

    const auto ec = ::GetModuleFileNameW(NULL, buffer.get_data(), buffer.get_size());

    if (ec == 0) {
        throw error::windows(GetLastError(), "GetModuleFileNameW");
    }

    if (ec == buffer.get_size() && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        buffer.grow();
        return get_current_executable_path(buffer);
    }

    return winapi::narrow(buffer.get_data());
}

std::string get_current_executable_path() {
    PathBuffer buffer;
    return get_current_executable_path(buffer);
}

std::string get_executable_path(const Handle& process, PathBuffer& buffer) {
    auto size = buffer.get_size();

    const auto ec = ::QueryFullProcessImageNameW(process.get(), 0, buffer.get_data(), &size);

    if (ec != 0) {
        return winapi::narrow(buffer.get_data());
    }

    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        buffer.grow();
        return get_executable_path(process, buffer);
    }

    throw error::windows(GetLastError(), "QueryFullProcessImageNameW");
}

std::string get_executable_path(const Handle& process) {
    PathBuffer buffer;
    return get_executable_path(process, buffer);
}

} // namespace

Process Process::current() {
    return Process{::GetCurrentProcessId(), Handle{::GetCurrentProcess()}};
}

Process Process::open(DWORD id) {
    return Process{id, open_process(id)};
}

Process::Process(Handle&& handle) : Process{::GetProcessId(handle.get()), std::move(handle)} {}

Process::Process(ID id, Handle&& handle) : id{id}, handle{std::move(handle)} {}

std::string Process::get_executable_path() const {
    return get_executable_path(handle);
}

std::string Process::get_executable_path(const Handle& handle) {
    if (handle.get() == ::GetCurrentProcess()) {
        return pdb::get_current_executable_path();
    } else {
        return pdb::get_executable_path(handle);
    }
}

} // namespace pdb
