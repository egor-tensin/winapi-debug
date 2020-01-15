namespace foobar_ns {

int exit_code = 1;

int __declspec(noinline) baz() {
    return exit_code;
}

int __declspec(noinline) bar() {
    return baz() * 2;
}

int __declspec(noinline) foo() {
    return bar() * 2;
}

} // namespace foobar_ns

int main() {
    return foobar_ns::foo() * 2;
}
