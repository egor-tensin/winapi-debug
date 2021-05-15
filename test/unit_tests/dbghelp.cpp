// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#include "fixtures.hpp"

#include <pdb/all.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <string>
#include <vector>

BOOST_FIXTURE_TEST_SUITE(enum_symbols_tests, PostMortem)

BOOST_AUTO_TEST_CASE(enum_symbols) {
    // Symbols can be enumerated, and all the expected symbols are there.

    // First, enumerate all the symbols:
    std::vector<std::string> all_symbols;
    {
        const auto callback = [&all_symbols](const pdb::SymbolInfo& symbol) {
            all_symbols.emplace_back(symbol.get_name());
        };
        dbghelp.enum_symbols(callback);
    }

    // Next, check that all the expected symbols are there:
    {
        const auto expected = expected_symbols();
        const auto check = [&all_symbols](const std::string& name) {
            return std::find(all_symbols.cbegin(), all_symbols.cend(), name) != all_symbols.cend();
        };
        for (const auto& name : expected) {
            BOOST_TEST(check(name), "Symbol found: " << name);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
