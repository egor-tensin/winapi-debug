# oldstable as of May '26
# Plus, I'm pretty sure it's required for things like CONFIGURE_DEPENDS and
# CMAKE_MSVC_RUNTIME_LIBRARY and stuff.
cmake_minimum_required(VERSION 3.25)

project(winapi_debug CXX)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (MSVC)
    add_compile_options(/MP /W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)

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

add_subdirectory(3rdparty/winapi/common EXCLUDE_FROM_ALL)

add_subdirectory(src)
add_subdirectory(test)

install(FILES LICENSE.txt DESTINATION share)
