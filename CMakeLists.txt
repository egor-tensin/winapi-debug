cmake_minimum_required(VERSION 3.5) # for Boost::* imported targets

project(winapi_debug CXX)
enable_testing()

if(MSVC)
    # This is an ugly hack to make the tests work.
    # With incremental linking, the &func address does not point to the
    # function code, but to a jmp instruction to the real code (or it's the
    # other way around, I don't know).
    # This is apparently caused by incremental linking, e.g.
    # https://stackoverflow.com/questions/2485336/address-of-function-is-not-actual-code-address
    string(REPLACE "/INCREMENTAL" "/INCREMENTAL:NO"
        CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
        "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
    string(REPLACE "/INCREMENTAL" "/INCREMENTAL:NO"
        CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
        "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}")
    string(REPLACE "/INCREMENTAL" "/INCREMENTAL:NO"
        CMAKE_EXE_LINKER_FLAGS_DEBUG
        "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
    string(REPLACE "/INCREMENTAL" "/INCREMENTAL:NO"
        CMAKE_SHARED_LINKER_FLAGS_DEBUG
        "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
endif()

include(cmake/common.cmake)

find_package(Boost REQUIRED)
add_subdirectory(3rdparty/winapi/common)

file(GLOB_RECURSE winapi_debug_include "include/*.hpp")
file(GLOB_RECURSE winapi_debug_src "src/*.cpp")
add_library(winapi_debug ${winapi_debug_include} ${winapi_debug_src})
target_compile_definitions(winapi_debug PUBLIC _NO_CVCONST_H)
target_include_directories(winapi_debug PUBLIC include/)
target_link_libraries(winapi_debug PRIVATE winapi_common winapi_utf8)
target_link_libraries(winapi_debug
    PUBLIC Boost::boost
    PRIVATE Boost::disable_autolinking)
target_link_libraries(winapi_debug PRIVATE dbghelp)

if(MINGW)
    # FILE_ID_INFO and friends require at least 0x0602:
    message("MINGW")
    target_compile_definitions(winapi_debug PUBLIC
        NTDDI_VERSION=NTDDI_WIN8
        _WIN32_WINNT=_WIN32_WINNT_WIN8)
endif()

if(MSVC_VERSION EQUAL 1900)
    # DbgHelp warnings on Visual Studio 2015:
    target_compile_options(winapi_debug PUBLIC /wd4091)
endif()

add_subdirectory(test)
add_subdirectory(utils)

install(FILES LICENSE.txt DESTINATION share)
