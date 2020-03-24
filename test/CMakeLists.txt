add_executable(call_stack call_stack.cpp)
target_link_libraries(call_stack PRIVATE pdb_repo)
target_link_libraries(call_stack PRIVATE Boost::nowide)

install(TARGETS call_stack RUNTIME DESTINATION bin/test)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:call_stack>" DESTINATION bin/test OPTIONAL)
endif()

add_executable(symbols symbols.cpp)

install(TARGETS symbols RUNTIME DESTINATION bin/test)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:symbols>" DESTINATION bin/test OPTIONAL)
endif()

add_subdirectory(unit_tests)
