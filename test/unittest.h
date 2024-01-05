#ifndef UNITTEST_MACROS
#define UNITTEST_MACROS

// indirection pattern
#define CAT(lhs, rhs) lhs##rhs
#define CONCAT(lhs, rhs) CAT(lhs, rhs)
#define STRING(macro) #macro

// unused variable name anew
#define ANEW(var) CONCAT(var, __LINE__)

#endif // !UNITTEST_MACROS

#undef unittest
#undef ensure

#ifndef UNITTEST_MAIN

#include <assert.h>

#define unittest(name) void ANEW(fn)(void)
#define ensure(expr) assert(expr)

#else // UNITTEST_MAIN

#include <assert.h>

#ifndef UNITTEST_VERBOSE

#define unittest(name) \
	for (unsigned unittest_once = 0; !unittest_once || (putchar('.'), 0); \
	     ++unittest_once)

#define ensure(expr) assert(expr)

#define summary() printf("\n\nAll tests passed!\n")

#else // UNITTEST_VERBOSE

#include <stdio.h>

#define unittest_position(file, line) file ":" STRING(line) ": "
#define unittest_refer unittest_position(__FILE__, __LINE__)

#define unittest_result(name, error) \
	((error) ? printf("unittest `" name "` had %u error(s)\n", error) \
		 : printf("unittest `" name "` passed\n"))

#define unittest_assert(name, ok) \
	((ok) ? 0 : (printf(unittest_refer "assertion `" name "` failed\n"), 1))

#define unittest(name) \
	putchar('\n'); \
	for (unsigned unittest_once = 0, unittest_error = 0; \
	     !unittest_once || (unittest_result(name, unittest_error), 0); \
	     ++unittest_once)

#define ensure(expr) unittest_error += unittest_assert(#expr, (expr))

#define summary() assert(1)

#endif // !UNITTEST_VERBOSE

#endif // !UNITTEST_MAIN
