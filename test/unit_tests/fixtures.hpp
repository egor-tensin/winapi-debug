#pragma once

#include "paths.hpp"

#include <pdb/all.hpp>

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include <iterator>
#include <string>
#include <unordered_set>
#include <utility>

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
        static const std::string name{"test_ns"};
        return name;
    }

    typedef Set<std::string> SymbolList;

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
