#include <pdb/all.hpp>

#include <boost/test/unit_test.hpp>

#include <windows.h>

#include <string>

BOOST_AUTO_TEST_SUITE(error_tests)

BOOST_AUTO_TEST_CASE(file_not_found) {
    const std::string actual{pdb::error::windows(ERROR_FILE_NOT_FOUND).what()};
    BOOST_TEST(actual == "The system cannot find the file specified.");
}

BOOST_AUTO_TEST_SUITE_END()
