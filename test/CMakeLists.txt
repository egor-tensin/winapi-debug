add_library(test_lib SHARED test_lib.cpp)
target_link_libraries(test_lib PRIVATE pdb_repo)
target_link_libraries(test_lib PRIVATE Boost::nowide)
target_include_directories(test_lib PUBLIC .)
target_compile_definitions(test_lib PRIVATE TEST_LIB_EXPORTS)

install(TARGETS test_lib RUNTIME DESTINATION bin/test)
install_pdbs(TARGETS test_lib DESTINATION bin/test)

add_executable(call_stack call_stack.cpp)
target_link_libraries(call_stack PRIVATE test_lib)
target_link_libraries(call_stack PRIVATE Boost::nowide)

install(TARGETS call_stack RUNTIME DESTINATION bin/test)
install_pdbs(TARGETS call_stack DESTINATION bin/test)

add_test(NAME call_stack COMMAND call_stack)

add_subdirectory(unit_tests)
