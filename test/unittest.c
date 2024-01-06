#ifndef UNITTEST_MAIN
#include "unittest.h"
#include <stdbool.h>
#include <stdlib.h>

#include "../lexer.h"
#include "../macros.h"
#include "../parser.h"

#endif /* ifndef UNITTEST_MAIN */

static const char8_t * str =
    cast(const char8_t *, "let num = 30;\n"
			  "let letter = 'a';\n"
			  "if (num <= 32)\n"
			  "\tnum + (letter - 'a') * 3\n"
			  "else 30");

unittest("lexer") {
	struct token expected[] = {
	    {TOKEN_LABEL, str + 0, 3},
	    {TOKEN_LABEL, str + 4, 3},
	    {'=', str + 8, 1},
	    {TOKEN_DECIMAL, str + 10, 2},
	    {';', str + 12, 1},
	    {TOKEN_LABEL, str + 14, 3},
	    {TOKEN_LABEL, str + 18, 6},
	    {'=', str + 25, 1},
	    {'\'', str + 28, 1},
	    {';', str + 30, 1},
	    {TOKEN_LABEL, str + 32, 2},
	    {'(', str + 35, 1},
	    {TOKEN_LABEL, str + 36, 3},
	    {digraph('<'), str + 40, 2},
	    {TOKEN_DECIMAL, str + 43, 2},
	    {')', str + 45, 1},
	    {TOKEN_LABEL, str + 48, 3},
	    {'+', str + 52, 1},
	    {'(', str + 54, 1},
	    {TOKEN_LABEL, str + 55, 6},
	    {'-', str + 62, 1},
	    {'\'', str + 65, 1},
	    {')', str + 67, 1},
	    {'*', str + 69, 1},
	    {TOKEN_DECIMAL, str + 71, 1},
	    {TOKEN_LABEL, str + 73, 4},
	    {TOKEN_DECIMAL, str + 78, 2},
	};

	struct lexer lexer = {.ptr = str};
	struct token token = {0};
	for (size_t index = 0; (token = lex(&lexer)).type != TOKEN_EOF;
	     ++index) {
		struct token expect_token = expected[index];
		ensure((expect_token.type == token.type) &
		       (expect_token.ptr == token.ptr) &
		       (expect_token.span == token.span));
	}
}

unittest("parser") {

	struct node expected[] = {
	    {{TOKEN_ASCII + KEYWORD_LET, str + 0, 3}, 0},
	    {{TOKEN_LABEL, str + 4, 3}, 0},
	    {{'=', str + 8, 1}, 0},
	    {{TOKEN_DECIMAL, str + 10, 2}, 0},
	    {{TOKEN_ASCII + KEYWORD_LET, str + 14, 3}, 0},
	    {{TOKEN_LABEL, str + 18, 6}, 0},
	    {{'=', str + 25, 1}, 0},
	    {{'\'', str + 28, 1}, 0},
	    {{TOKEN_ASCII + KEYWORD_IF, str + 32, 2}, 0},
	    {{TOKEN_LABEL, str + 36, 3}, 0},
	    {{digraph('<'), str + 40, 2}, 0},
	    {{TOKEN_DECIMAL, str + 43, 2}, 0},
	    {{TOKEN_LABEL, str + 48, 3}, 0},
	    {{'+', str + 52, 1}, 0},
	    {{TOKEN_LABEL, str + 55, 6}, 0},
	    {{'-', str + 62, 1}, 0},
	    {{'\'', str + 65, 1}, 0},
	    {{'*', str + 69, 1}, 0},
	    {{TOKEN_DECIMAL, str + 71, 1}, 0},
	    {{TOKEN_ASCII + KEYWORD_ELSE, str + 73, 4}, 0},
	    {{TOKEN_DECIMAL, str + 78, 2}, 0},
	};

	struct node node = {0};
	struct parser parser = {.lexer = (struct lexer){.ptr = str},
				.arena = alloc(256U * sizeof(struct node))};
	error_t error = parse(&parser);
	ensure(error == ERROR_NONE || error == ERROR_EOF);

	for (size_t index = 0; index != sizeof(expected) / sizeof(struct node);
	     ++index) {
		struct token token =
		    ((struct node *) parser.arena.ptr)[index].token;
		struct token expect_token = expected[index].token;
		ensure((expect_token.type == token.type) &
		       (expect_token.ptr == token.ptr) &
		       (expect_token.span == token.span));
	}

	dealloc(&parser.arena);
}
