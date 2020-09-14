#pragma once

// This is dumb library which exports a bunch of symbols.

#include "test_lib_api.hpp"

namespace test_ns {

typedef void (*F)();

TEST_LIB_API extern volatile int var;

TEST_LIB_API void foo(F);
TEST_LIB_API void bar(F);
TEST_LIB_API void baz(F);

TEST_LIB_API void print_call_stack();
TEST_LIB_API void throw_call_stack();

} // namespace test_ns
