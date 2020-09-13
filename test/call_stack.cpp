#include "pdb/all.hpp"

#include <boost/config.hpp>
#include <boost/nowide/iostream.hpp>

#include <exception>

namespace test {

void call_stack() {
    const auto dbghelp = pdb::DbgHelp::current_process();
    const auto call_stack = pdb::CallStack::capture();
    call_stack.dump(boost::nowide::cout, dbghelp);
}

// Some tricks to prevent the functions from being inlined follow...
BOOST_NOINLINE void baz() {
    boost::nowide::cout << "baz " << &baz << '\n';
    call_stack();
}

BOOST_NOINLINE void bar() {
    boost::nowide::cout << "bar " << &bar << '\n';
    baz();
}

BOOST_NOINLINE void foo() {
    boost::nowide::cout << "foo " << &foo << '\n';
    bar();
}

} // namespace test

int main() {
    try {
        test::foo();
    } catch (const std::exception& e) {
        boost::nowide::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
