// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include "workarounds.hpp"

#include <windows.h>

#include <cassert>
#include <memory>

namespace pdb {

struct CloseHandle {
    void operator()(HANDLE raw) const {
        if (raw == NULL || raw == INVALID_HANDLE_VALUE)
            return;
        const auto ret = ::CloseHandle(raw);
        assert(ret);
        PDB_UNUSED_PARAMETER(ret);
    }
};

typedef std::unique_ptr<void, CloseHandle> Handle;

} // namespace pdb
