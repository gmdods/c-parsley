#ifndef LEXER_H
#define LEXER_H

#include "glyph.h"

enum token_type /* : char8_t */ {
	TOKEN_EOF = 0x0,
	TOKEN_BAD = 0x1,
	TOKEN_COMMENT = '#',
	TOKEN_DECIMAL = '0',
	TOKEN_LABEL = '_',
	TOKEN_ASCII = ASCII_LIMIT,

	TOKEN_DIGRAPH = 0xa0,
	TOKEN_UTF8 = UTF8_INIT,
};

#define digraph(ch) (0xa0 | ((ch) & 0x1f))

struct token {
	char8_t /* enum token */ type;
	const char8_t * ptr;
	size_t span;
};

struct lexer {
	const char8_t * ptr;
};

struct token lex(struct lexer *);

#endif // !LEXER_H
