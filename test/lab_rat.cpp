#include <iostream>

namespace lab_rat {

void baz() {
    std::cout << "baz\n";
}

void bar() {
    std::cout << "bar\n";
    baz();
}

void foo() {
    std::cout << "foo\n";
    bar();
}

} // namespace lab_rat

int main() {
    lab_rat::foo();
    return 0;
}
