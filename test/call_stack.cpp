#include "pdb/all.hpp"

#include <exception>
#include <iostream>

namespace test {

void call_stack() {
    const auto dbghelp = pdb::DbgHelp::current_process();
    const auto call_stack = pdb::CallStack::capture();
    call_stack.dump(std::cout, dbghelp);
}

void __declspec(noinline) baz() {
    std::cout << "baz\n";
    call_stack();
}

void __declspec(noinline) bar() {
    std::cout << "bar\n";
    baz();
}

void __declspec(noinline) foo() {
    std::cout << "foo\n";
    bar();
}

} // namespace test

int main() {
    try {
        test::foo();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
