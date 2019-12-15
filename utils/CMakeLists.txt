find_package(Boost REQUIRED COMPONENTS filesystem program_options system)

add_executable(enum_symbols enum_symbols.cpp command_line.hpp pdb_descr.hpp)
target_include_directories(enum_symbols SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(enum_symbols PRIVATE pdb_repo ${Boost_LIBRARIES})

add_executable(name2addr name2addr.cpp)
target_include_directories(name2addr SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(name2addr PRIVATE pdb_repo ${Boost_LIBRARIES})

add_executable(addr2name addr2name.cpp command_line.hpp pdb_descr.hpp)
target_include_directories(addr2name SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(addr2name PRIVATE pdb_repo ${Boost_LIBRARIES})

install(TARGETS enum_symbols name2addr addr2name RUNTIME DESTINATION bin)
