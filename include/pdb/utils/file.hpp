// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#pragma once

#include <boost/functional/hash.hpp>

#include <windows.h>

#include <cstddef>
#include <cstring>
#include <functional>
#include <string>

namespace pdb {
namespace file {

std::size_t get_size(const std::string&);

inline bool operator==(const FILE_ID_128& a, const FILE_ID_128& b) {
    return 0 == std::memcmp(a.Identifier, b.Identifier, sizeof(a.Identifier));
}

struct ID {
    const FILE_ID_INFO raw;

    bool operator==(const ID& other) const {
        return raw.VolumeSerialNumber == other.raw.VolumeSerialNumber &&
               raw.FileId == other.raw.FileId;
    }
};

ID query_id(const std::string&);

} // namespace file
} // namespace pdb

namespace std {

template <>
struct hash<pdb::file::ID> {
    std::size_t operator()(const pdb::file::ID& id) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, id.raw.VolumeSerialNumber);
        boost::hash_combine(seed, id.raw.FileId.Identifier);
        return seed;
    }
};

} // namespace std
