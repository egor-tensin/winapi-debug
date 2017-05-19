// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include <Windows.h>

#include <cstddef>
#include <cstring>

#include <functional>
#include <string>
#include <type_traits>

namespace pdb
{
    namespace file
    {
        std::size_t get_size(const std::string&);

        struct ID
        {
            const FILE_ID_INFO raw;

            bool operator==(const ID& other) const
            {
                static_assert(std::is_pod<FILE_ID_INFO>::value, "Can't memcmp if file IDs aren't PODs");
                return 0 == std::memcmp(&raw, &other.raw, sizeof(FILE_ID_INFO));
            }
        };

        ID query_id(const std::string&);
    }
}

namespace std
{
    template <>
    struct hash<pdb::file::ID>
    {
        std::size_t operator()(const pdb::file::ID& id) const
        {
            return _Bitwise_hash<FILE_ID_INFO>{}(id.raw);
        }
    };
}
