#include "fixtures.hpp"
#include "paths.hpp"

#include <pdb/all.hpp>
#include <test_lib.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <string>
#include <vector>

namespace {

void throw_call_stack() {
    throw pdb::CallStack::capture();
}

} // namespace

BOOST_FIXTURE_TEST_SUITE(dbghelp_tests, DbgHelpWithSymbols)

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
            BOOST_TEST(check(name), "Symbol wasn't enumerated: " << name);
        }
    }
}

BOOST_AUTO_TEST_CASE(call_stack) {
    try {
        test::foo(&throw_call_stack);
    } catch (const pdb::CallStack& call_stack) {
        BOOST_TEST(true, "Caught the call stack");
        return;
    }
    BOOST_TEST(false, "Didn't catch the call stack");
}

BOOST_AUTO_TEST_SUITE_END()
