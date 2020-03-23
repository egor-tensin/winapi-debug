#include "pdb/all.hpp"

#include <boost/nowide/iostream.hpp>

#include <exception>

namespace test {

void call_stack() {
    const auto dbghelp = pdb::DbgHelp::current_process();
    const auto call_stack = pdb::CallStack::capture();
    call_stack.dump(boost::nowide::cout, dbghelp);
}

void __declspec(noinline) baz() {
    boost::nowide::cout << "baz\n";
    call_stack();
}

void __declspec(noinline) bar() {
    boost::nowide::cout << "bar\n";
    baz();
}

void __declspec(noinline) foo() {
    boost::nowide::cout << "foo\n";
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
