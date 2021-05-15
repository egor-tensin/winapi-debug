find_package(Boost REQUIRED COMPONENTS filesystem program_options)

function(add_util name src)
    add_executable("${name}" ${src})
    target_link_libraries("${name}" PRIVATE pdb_repo)
    target_link_libraries("${name}" PRIVATE Boost::disable_autolinking Boost::filesystem Boost::program_options)
    install(TARGETS "${name}" RUNTIME DESTINATION bin)
    install_pdbs(TARGETS "${name}" DESTINATION bin)
endfunction()

add_util(enum_symbols enum_symbols.cpp command_line.hpp pdb_descr.hpp)
add_util(name2addr name2addr.cpp)
add_util(addr2name addr2name.cpp command_line.hpp pdb_descr.hpp)

if(MSVC)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_test(NAME enum_symbols COMMAND Python3::Interpreter
        "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/tools/ctest-driver.py"
        run
        --pass-regex test_ns::baz test_ns::bar test_ns::foo test_ns::var
        --
        "$<TARGET_FILE:enum_symbols>"
        --pdb "$<TARGET_PDB_FILE:test_lib>"
        --mask "test_ns:*")
endif()
