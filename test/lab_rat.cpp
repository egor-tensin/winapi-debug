#include <iostream>

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

int main() {
    foo();
    return 0;
}
