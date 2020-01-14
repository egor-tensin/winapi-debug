cmake_minimum_required(VERSION 3.5) # for Boost::* imported targets

project(pdb_repo CXX)

include(cmake/cmake/common.cmake)

find_package(Boost REQUIRED)

file(GLOB_RECURSE pdb_repo_include "include/*.hpp")
file(GLOB_RECURSE pdb_repo_src "src/*.cpp")
add_library(pdb_repo ${pdb_repo_include} ${pdb_repo_src})
target_compile_definitions(pdb_repo PUBLIC _NO_CVCONST_H)
target_include_directories(pdb_repo PUBLIC include/)
target_include_directories(pdb_repo SYSTEM PUBLIC 3rdparty/microsoft/SafeInt)
target_link_libraries(pdb_repo PUBLIC Boost::boost)
target_link_libraries(pdb_repo PRIVATE DbgHelp)

if(MINGW)
    # FILE_ID_INFO and friends require at least 0x0602:
    target_compile_definitions(pdb_repo PUBLIC
        NTDDI_VERSION=NTDDI_WIN8
        _WIN32_WINNT=_WIN32_WINNT_WIN8)
endif()

add_subdirectory(utils)

install(FILES LICENSE.txt DESTINATION share)
