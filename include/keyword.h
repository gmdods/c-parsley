#ifndef KEYWORD_H
#define KEYWORD_H

#include "defs.h"
#include "macros.h"
#include <string.h>

enum keywords {
	KEYWORD_RETURN,
	KEYWORD_IF,
	KEYWORD_THEN,
	KEYWORD_ELSE,
	KEYWORD_WHILE,
	KEYWORD_BREAK,
	// Not C
	KEYWORD_LET,
	KEYWORD_LOOP,
	// END
	KEYWORD_NONE,
};

struct string {
	const char8_t * ptr;
	size_t span;
};

#define string_literal(str) \
	{ .ptr = cast(const char8_t *, str), .span = sizeof(str) - 1 }

static struct string keywords[] = {
    [KEYWORD_RETURN] = string_literal("return"),
    [KEYWORD_IF] = string_literal("if"),
    [KEYWORD_THEN] = string_literal("then"),
    [KEYWORD_ELSE] = string_literal("else"),
    [KEYWORD_WHILE] = string_literal("while"),
    [KEYWORD_BREAK] = string_literal("break"),

    [KEYWORD_LET] = string_literal("let"),
    [KEYWORD_LOOP] = string_literal("loop"),
};

#endif // !KEYWORD_H
