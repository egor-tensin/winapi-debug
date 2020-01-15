set(CC_STATIC_RUNTIME OFF)
add_executable(lab_rat lab_rat.cpp)

install(TARGETS lab_rat RUNTIME DESTINATION bin)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:lab_rat>" DESTINATION bin OPTIONAL)
endif()
