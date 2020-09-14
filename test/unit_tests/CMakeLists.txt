find_package(Boost REQUIRED COMPONENTS filesystem unit_test_framework)

add_executable(unit_tests main.cpp call_stack.cpp dbghelp.cpp error.cpp)
target_link_libraries(unit_tests PRIVATE pdb_repo test_lib)
target_link_libraries(unit_tests PRIVATE Boost::disable_autolinking Boost::filesystem Boost::nowide Boost::unit_test_framework)

install(TARGETS unit_tests RUNTIME DESTINATION bin/test)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:unit_tests>" DESTINATION bin/test OPTIONAL)
endif()
