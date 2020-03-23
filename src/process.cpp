#include "pdb/all.hpp"

#include <boost/nowide/convert.hpp>

#include <SafeInt.hpp>

#include <windows.h>

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
        throw error::windows(GetLastError());
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
            if (!SafeMultiply(size, 2, size)) {
                throw std::range_error{"couldn't allocate buffer sufficient for a file path"};
            }
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
        throw error::windows(GetLastError());
    }

    if (ec == buffer.get_size() && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        buffer.grow();
        return get_current_executable_path(buffer);
    }

    return boost::nowide::narrow(buffer.get_data());
}

std::string get_current_executable_path() {
    PathBuffer buffer;
    return get_current_executable_path(buffer);
}

std::string get_executable_path(const Handle& process, PathBuffer& buffer) {
    auto size = buffer.get_size();

    const auto ec = ::QueryFullProcessImageNameW(process.get(), 0, buffer.get_data(), &size);

    if (ec != 0) {
        return boost::nowide::narrow(buffer.get_data());
    }

    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        buffer.grow();
        return get_executable_path(process, buffer);
    }

    throw error::windows(GetLastError());
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
    if (handle.get() == ::GetCurrentProcess()) {
        return pdb::get_current_executable_path();
    } else {
        return pdb::get_executable_path(handle);
    }
}

} // namespace pdb