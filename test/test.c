#include <stdbool.h>

#include "../include/lexer.h"
#include "../include/macros.h"
#include "../include/parser.h"

#define UNITTEST_MAIN
#include "unittest.h"

int main(int argc, const char * argv[]) {

#include "unittest.c"

	summary();
	return 0;
}
