#include "pdb/all.hpp"

#include <boost/config.hpp>
#include <boost/nowide/iostream.hpp>

#include <exception>

namespace test {

typedef void (*F)();

void call_stack() {
    const auto dbghelp = pdb::DbgHelp::current_process();
    const auto call_stack = pdb::CallStack::capture();
    call_stack.dump(boost::nowide::cout, dbghelp);
}

// Some tricks to prevent the functions from being inlined follow...
void baz() {
    boost::nowide::cout << "baz " << &baz << '\n';
    F f = &call_stack;
    f();
}

void bar() {
    boost::nowide::cout << "bar " << &bar << '\n';
    F f = &baz;
    f();
}

void foo() {
    boost::nowide::cout << "foo " << &foo << '\n';
    F f = &bar;
    f();
}

} // namespace test

int main() {
    try {
        test::F f = &test::foo;
        f();
    } catch (const std::exception& e) {
        boost::nowide::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
