#include <stdbool.h>

#include "../lexer.h"
#include "../macros.h"
#include "../parser.h"

#define UNITTEST_MAIN
#include "unittest.h"

int main(int argc, const char * argv[]) {

#include "unittest.c"

	summary();
	return 0;
}
