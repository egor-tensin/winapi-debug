// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#pragma once

#include "handle.hpp"

#include <windows.h>

#include <string>

namespace pdb {

class Process {
public:
    using ID = DWORD;

    static Process current();
    static Process open(ID);

    ID get_id() const { return id; }

    const Handle& get_handle() const { return handle; }

    std::string get_executable_path() const;
    static std::string get_executable_path(const Handle&);

private:
    explicit Process(Handle&&);
    Process(ID, Handle&&);

    ID id;
    Handle handle;
};

} // namespace pdb
