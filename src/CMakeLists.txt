find_package(Boost CONFIG REQUIRED)

file(GLOB_RECURSE winapi_debug_include CONFIGURE_DEPENDS "../include/*.hpp")
file(GLOB winapi_debug_src CONFIGURE_DEPENDS "*.cpp")
add_library(winapi_debug ${winapi_debug_include} ${winapi_debug_src})
target_compile_definitions(winapi_debug PUBLIC _NO_CVCONST_H)
target_include_directories(winapi_debug PUBLIC ../include/)
target_link_libraries(winapi_debug
    PUBLIC winapi_common
    PRIVATE winapi_utf8)
target_link_libraries(winapi_debug
    PUBLIC Boost::boost
    PRIVATE Boost::disable_autolinking)
target_link_libraries(winapi_debug PRIVATE dbghelp)
INSTALL(TARGETS winapi_debug ARCHIVE DESTINATION lib)
install(DIRECTORY ../include/winapi DESTINATION include)

if(MSVC_VERSION EQUAL 1900)
    # DbgHelp warnings on Visual Studio 2015:
    target_compile_options(winapi_debug PUBLIC /wd4091)
endif()

add_subdirectory(utils)
