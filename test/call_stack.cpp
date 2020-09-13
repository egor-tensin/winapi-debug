#include <test_lib.hpp>

#include <boost/nowide/iostream.hpp>

#include <exception>

int main() {
    try {
        test::print_call_stack();
    } catch (const std::exception& e) {
        boost::nowide::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
