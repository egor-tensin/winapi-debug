#include "test_lib.hpp"

#include <pdb/all.hpp>

#include <boost/nowide/iostream.hpp>

// Prevent frame pointer omission (FPO) and/or inlining.
#ifdef _MSC_VER
#pragma optimize("", off)
#endif

namespace test {
namespace {

void do_print_call_stack() {
    const auto dbghelp = pdb::DbgHelp::current_process();
    const auto call_stack = pdb::CallStack::capture();
    call_stack.dump(boost::nowide::cout, dbghelp);
}

} // namespace

volatile int var = 42;

void baz(F f) {
    boost::nowide::cout << "baz\n";
    f();
}

void bar(F f) {
    boost::nowide::cout << "bar\n";
    baz(f);
}

void foo(F f) {
    boost::nowide::cout << "foo\n";
    bar(f);
}

void print_call_stack() {
    foo(&do_print_call_stack);
}

} // namespace test
