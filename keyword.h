#ifndef KEYWORD_H
#define KEYWORD_H

#include <string.h>

#include "glyph.h"
#include "macros.h"

enum keywords {
	KEYWORD_LET,
	KEYWORD_IF,
	KEYWORD_ELSE,
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

#define keyword(token, keyword) \
	(((token).span == keywords[keyword].span) && \
	 (strncmp((const char *) (token).ptr, \
		  (const char *) keywords[keyword].ptr, \
		  keywords[keyword].span) == 0))

#endif // !KEYWORD_H
