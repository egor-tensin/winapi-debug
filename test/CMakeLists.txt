set(CC_STATIC_RUNTIME OFF)
add_executable(foobar foobar.cpp)

install(TARGETS foobar RUNTIME DESTINATION bin)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:foobar>" DESTINATION bin OPTIONAL)
endif()
