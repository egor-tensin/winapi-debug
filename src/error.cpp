// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include "pdb/all.hpp"

#include <Windows.h>

#include <string>

namespace pdb
{
    namespace error
    {
        namespace
        {
            std::string trim_trailing_newline(const std::string& s)
            {
                const auto last_pos = s.find_last_not_of("\r\n");
                if (std::string::npos == last_pos)
                    return {};
                return s.substr(0, last_pos + 1);
            }
        }

        std::string CategoryWindows::message(int code) const
        {
            char* buf;

            const auto nbwritten = FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER
                    | FORMAT_MESSAGE_FROM_SYSTEM
                    | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                code,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                reinterpret_cast<char*>(&buf),
                0,
                NULL);

            if (0 == nbwritten)
            {
                LocalFree(buf);
                return "Couldn't format the error message";
            }

            std::string msg{buf, nbwritten};
            LocalFree(buf);
            return trim_trailing_newline(msg);
        }
    }
}
