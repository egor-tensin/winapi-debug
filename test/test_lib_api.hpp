#pragma once

#ifdef TEST_LIB_EXPORTS
#define TEST_LIB_API __declspec(dllexport)
#else
#define TEST_LIB_API __declspec(dllimport)
#endif
