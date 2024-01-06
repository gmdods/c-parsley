#ifndef KEYWORD_H
#define KEYWORD_H

#include "glyph.h"
#include "macros.h"
#include <string.h>

enum keywords {
	KEYWORD_LET,
	KEYWORD_IF,
	KEYWORD_ELSE,
	KEYWORD_NONE,
};

struct string {
	const char8_t * ptr;
	size_t span;
};

#define string_literal(str) \
	{ .ptr = cast(const char8_t *, str), .span = sizeof(str) - 1 }

static struct string keywords[] = {
    [KEYWORD_LET] = string_literal("let"),
    [KEYWORD_IF] = string_literal("if"),
    [KEYWORD_ELSE] = string_literal("else"),
};

#endif // !KEYWORD_H
