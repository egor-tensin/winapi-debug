// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#ifdef TEST_LIB_EXPORTS
#define TEST_LIB_API __declspec(dllexport)
#else
#define TEST_LIB_API __declspec(dllimport)
#endif
