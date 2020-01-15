namespace lab_rat {

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

} // namespace lab_rat

int main() {
    return lab_rat::foo() * 2;
}
