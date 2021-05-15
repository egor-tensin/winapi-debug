add_library(test_lib SHARED test_lib.cpp)
target_link_libraries(test_lib PRIVATE winapi_debug)
target_include_directories(test_lib PUBLIC .)
target_compile_definitions(test_lib PRIVATE TEST_LIB_EXPORTS)
install(TARGETS test_lib RUNTIME DESTINATION bin/test)
install_pdbs(TARGETS test_lib DESTINATION bin/test)

add_executable(call_stack call_stack.cpp)
target_link_libraries(call_stack PRIVATE test_lib)
install(TARGETS call_stack RUNTIME DESTINATION bin/test)
install_pdbs(TARGETS call_stack DESTINATION bin/test)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_test(NAME call_stack COMMAND Python3::Interpreter
    "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/tools/ctest-driver.py"
    run
    --pass-regex test_ns::baz test_ns::bar test_ns::foo
    --
    "$<TARGET_FILE:call_stack>")

add_subdirectory(unit_tests)
