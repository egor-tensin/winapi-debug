#include "utils.hpp"

#include <pdb/all.hpp>

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include <iterator>
#include <string>
#include <unordered_set>
#include <utility>

namespace {

class DbgHelp {
public:
    DbgHelp() : dbghelp{pdb::DbgHelp::post_mortem()} { BOOST_TEST_MESSAGE("Initializing DbgHelp"); }

    ~DbgHelp() { BOOST_TEST_MESSAGE("Cleaning up DbgHelp"); }

    const pdb::DbgHelp dbghelp;

private:
    DbgHelp(const DbgHelp&) = delete;
    DbgHelp& operator=(const DbgHelp&) = delete;
};

template <typename T>
using Set = std::unordered_set<T>;

template <typename T>
Set<T> join(Set<T>&& xs, Set<T>&& ys) {
    xs.insert(std::make_move_iterator(ys.begin()), std::make_move_iterator(ys.end()));
    return std::move(xs);
}

class DbgHelpWithSymbols : public DbgHelp {
public:
    DbgHelpWithSymbols() { load_test_lib_pdb(); }

    static const std::string& get_namespace() {
        static const std::string name{"test"};
        return name;
    }

    typedef std::unordered_set<std::string> SymbolList;

    static SymbolList expected_functions() { return make_qualified({"foo", "bar", "baz"}); }

    static SymbolList expected_variables() { return make_qualified({"var"}); }

    static SymbolList expected_symbols() {
        return join(expected_functions(), expected_variables());
    }

private:
    static SymbolList make_qualified(SymbolList&& plain) {
        SymbolList qualified;
        for (auto&& name : plain) {
            qualified.emplace(get_namespace() + "::" + std::move(name));
        }
        return qualified;
    }

    void load_test_lib_pdb() {
        const auto pdb_path = get_test_lib_pdb_path().string();
        BOOST_TEST_MESSAGE("Loading PDB: " << pdb_path);
        dbghelp.load_pdb(pdb_path);
    }

    static boost::filesystem::path get_test_lib_pdb_path() {
        return Paths::get().exe_dir / "test_lib.pdb";
    }
};

} // namespace

BOOST_AUTO_TEST_SUITE(dbghelp_tests)
BOOST_FIXTURE_TEST_SUITE(enum_symbols_tests, DbgHelpWithSymbols)

BOOST_AUTO_TEST_CASE(basic) {
    // Symbols can be enumerated, and all the expected symbols are there.
    auto all_symbols = expected_symbols();
    const auto callback = [&all_symbols](const pdb::SymbolInfo& symbol) {
        const auto name = symbol.get_name();
        all_symbols.erase(name);
    };
    dbghelp.enum_symbols(callback);
    for (const auto& missing : all_symbols) {
        BOOST_TEST(false, "Symbol wasn't enumerated: " << missing);
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
