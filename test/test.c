#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lexer.h"
#include "../macros.h"

#define UNITTEST_MAIN
#include "unittest.h"

int main(int argc, const char * argv[]) {

#include "unittest.c"

	summary();
	return 0;
}
