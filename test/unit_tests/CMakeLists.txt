find_package(Boost CONFIG REQUIRED COMPONENTS unit_test_framework)

add_executable(unit_tests main.cpp call_stack.cpp dbghelp.cpp)
target_link_libraries(unit_tests PRIVATE test_lib winapi_debug)
target_link_libraries(unit_tests PRIVATE
    Boost::disable_autolinking
    Boost::unit_test_framework)
install(TARGETS unit_tests RUNTIME DESTINATION bin/test)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:unit_tests> DESTINATION bin/test OPTIONAL)
endif()

if(MSVC)
    # Set the working directory so that the unit_tests binary can find the
    # test_lib shared library binary.
    add_test(NAME unit_tests COMMAND unit_tests
         --log_level=all
         --no_color_output
         --
         --test_lib_pdb "$<TARGET_PDB_FILE:test_lib>"
         WORKING_DIRECTORY "$<TARGET_FILE_DIR:test_lib>")
endif()
