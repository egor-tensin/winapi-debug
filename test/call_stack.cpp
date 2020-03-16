#include "pdb/all.hpp"

#include <exception>
#include <iostream>

namespace test {

void call_stack() {
    const auto dbghelp = pdb::DbgHelp::current_process();
    const auto call_stack = pdb::CallStack::capture();
    for (std::size_t i = 0; i < call_stack.length; ++i)
        std::cout << pdb::format_address(call_stack.frames[i]) << ' '
                  << pdb::call_stack::pretty_print_address(dbghelp, call_stack.frames[i]) << '\n';
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
