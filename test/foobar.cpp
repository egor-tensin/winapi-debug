namespace foobar_ns {

int exit_code = 1;

int baz() {
    return exit_code;
}

int bar() {
    return baz() * 2;
}

int foo() {
    return bar() * 2;
}

} // namespace foobar_ns

int main() {
    return foobar_ns::foo() * 2;
}
