#ifndef UNITTEST_MAIN
#include "unittest.h"
#include <stdbool.h>
#include <stdlib.h>

#include "../include/lexer.h"
#include "../include/macros.h"
#include "../include/parser.h"

#endif /* ifndef UNITTEST_MAIN */

static const char8_t * str =
    cast(const char8_t *, "let num = 30;\n"
			  "let letter = 'a';\n"
			  "if (num <= 32)\n"
			  "then num + (letter - 'a') * 3\n"
			  "else 30");

unittest("lexer") {
	struct token expected[] = {
	    {tokenof(KEYWORD_LET), 0, 3},
	    {TOKEN_LABEL, 4, 3},
	    {'=', 8, 1},
	    {TOKEN_DECIMAL, 10, 2},
	    {';', 12, 1},
	    {tokenof(KEYWORD_LET), 14, 3},
	    {TOKEN_LABEL, 18, 6},
	    {'=', 25, 1},
	    {'\'', 28, 1},
	    {';', 30, 1},
	    {tokenof(KEYWORD_IF), 32, 2},
	    {'(', 35, 1},
	    {TOKEN_LABEL, 36, 3},
	    {digraph('<'), 40, 2},
	    {TOKEN_DECIMAL, 43, 2},
	    {')', 45, 1},
	    {tokenof(KEYWORD_THEN), 47, 4},
	    {TOKEN_LABEL, 52, 3},
	    {'+', 56, 1},
	    {'(', 58, 1},
	    {TOKEN_LABEL, 59, 6},
	    {'-', 66, 1},
	    {'\'', 69, 1},
	    {')', 71, 1},
	    {'*', 73, 1},
	    {TOKEN_DECIMAL, 75, 1},
	    {tokenof(KEYWORD_ELSE), 77, 4},
	    {TOKEN_DECIMAL, 82, 2},
	};

	struct lexer lexer = {.ptr = str};
	struct token token = {0};
	for (size_t index = 0; (token = lex(&lexer)).type != TOKEN_EOF;
	     ++index) {
		struct token expect_token = expected[index];
		ensure(expect_token.type == token.type),
		    ensure(expect_token.init == token.init),
		    ensure(expect_token.span == token.span);
	}
}

unittest("parser") {

	struct node expected[] = {
	    {0, {tokenof(KEYWORD_LET), 0, 3}},
	    {1, {TOKEN_LABEL, 4, 3}},
	    {1, {'=', 8, 1}},
	    {1, {TOKEN_DECIMAL, 10, 2}},
	    {0, {tokenof(KEYWORD_LET), 14, 3}},
	    {5, {TOKEN_LABEL, 18, 6}},
	    {5, {'=', 25, 1}},
	    {5, {'\'', 28, 1}},
	    {0, {tokenof(KEYWORD_IF), 32, 2}},
	    {9, {'(', 35, 1}},
	    {10, {TOKEN_LABEL, 36, 3}},
	    {10, {digraph('<'), 40, 2}},
	    {12, {TOKEN_DECIMAL, 43, 2}},
	    {9, {tokenof(KEYWORD_THEN), 47, 4}},
	    {14, {TOKEN_LABEL, 52, 3}},
	    {14, {'+', 56, 1}},
	    {16, {'(', 58, 1}},
	    {17, {TOKEN_LABEL, 59, 6}},
	    {17, {'-', 66, 1}},
	    {19, {'\'', 69, 1}},
	    {17, {'*', 73, 1}},
	    {21, {TOKEN_DECIMAL, 75, 1}},
	    {9, {tokenof(KEYWORD_ELSE), 77, 4}},
	    {23, {TOKEN_DECIMAL, 82, 2}},
	};

	struct node node = {0};
	struct parser parser = {.lexer = (struct lexer){.ptr = str},
				.arena = alloc(256U * sizeof(struct node))};
	error_t error = parse(&parser);
	ensure(error == ERROR_EOF);

	for (size_t index = 0; index != sizeof(expected) / sizeof(struct node);
	     ++index) {
		struct node node = ((struct node *) parser.arena.ptr)[index];
		struct node expect = expected[index];
		ensure(expect.token.type == node.token.type),
		    ensure(expect.token.init == node.token.init),
		    ensure(expect.token.span == node.token.span),
		    ensure(expect.index == node.index);
	}

	dealloc(&parser.arena);
}

unittest("parser errors") {
	static const char8_t * issue =
	    cast(const char8_t *, "while (1) { break; } != 1");

	struct parser parser = {.lexer = (struct lexer){.ptr = issue},
				.arena = alloc(256U * sizeof(struct node))};
	error_t error = parse(&parser);
	ensure(error == ERROR_BAD), ensure(parser.token.type == digraph('!')),
	    ensure(parser.token.span == 2), ensure(parser.token.init == 21);

	dealloc(&parser.arena);
}
