find_package(Boost CONFIG REQUIRED COMPONENTS program_options)

file(GLOB headers CONFIGURE_DEPENDS "*.hpp")

function(add_util name src)
    add_executable("${name}" ${src} ${headers})
    target_link_libraries("${name}" PRIVATE winapi_debug)
    target_link_libraries("${name}" PRIVATE
        Boost::disable_autolinking
        Boost::program_options
    )
    install(TARGETS "${name}" RUNTIME DESTINATION bin)
    if(MSVC)
        install(FILES "$<TARGET_PDB_FILE:${name}>" DESTINATION bin OPTIONAL)
    endif()
endfunction()

add_util(enum_symbols enum_symbols.cpp)
add_util(name2addr name2addr.cpp)
add_util(addr2name addr2name.cpp)

if(MSVC)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_test(NAME enum_symbols COMMAND Python3::Interpreter
        "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/tools/ctest-driver.py"
        run
        --pass-regex test_ns::baz test_ns::bar test_ns::foo test_ns::var
        --
        "$<TARGET_FILE:enum_symbols>"
        --pdb "$<TARGET_PDB_FILE:test_lib>"
        --mask "test_ns:*"
    )
endif()
