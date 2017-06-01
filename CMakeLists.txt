project(pdb_repo CXX)

include(common.cmake)

file(GLOB_RECURSE pdb_repo_include "include/*.hpp")
file(GLOB_RECURSE pdb_repo_src "src/*.cpp")
add_library(pdb_repo ${pdb_repo_include} ${pdb_repo_src})
target_compile_definitions(pdb_repo PRIVATE NOMINMAX PUBLIC _NO_CVCONST_H)
target_include_directories(pdb_repo PUBLIC include/)
target_link_libraries(pdb_repo PRIVATE DbgHelp)

add_subdirectory(utils)
