// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include "test_lib.hpp"

#include <pdb/all.hpp>

#include <boost/nowide/iostream.hpp>

// Prevent frame pointer omission (FPO) and/or inlining.
#ifdef _MSC_VER
#pragma optimize("", off)
#endif

namespace test_ns {
namespace {

void do_print_call_stack() {
    const auto dbghelp = pdb::DbgHelp::current_process();
    const auto call_stack = pdb::CallStack::capture();
    call_stack.dump(boost::nowide::cout, dbghelp);
}

void do_throw_call_stack() {
    throw pdb::CallStack::capture();
}

} // namespace

volatile int var = 42;

void baz(F f) {
    boost::nowide::cout << "baz " << pdb::format_address(reinterpret_cast<void*>(&baz)) << '\n';
    f();
}

void bar(F f) {
    boost::nowide::cout << "bar " << pdb::format_address(reinterpret_cast<void*>(&bar)) << '\n';
    baz(f);
}

void foo(F f) {
    boost::nowide::cout << "foo " << pdb::format_address(reinterpret_cast<void*>(&foo)) << '\n';
    bar(f);
}

void print_call_stack() {
    foo(&do_print_call_stack);
}

void throw_call_stack() {
    foo(&do_throw_call_stack);
}

} // namespace test_ns
