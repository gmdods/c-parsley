#ifndef PARSER_H
#define PARSER_H

#include "glyph.h"
#include "keyword.h"
#include "lexer.h"
#include "memory.h"

typedef enum error /* : char8_t */ {
	ERROR_NONE = 0,
	ERROR_EOF,
	ERROR_BAD,
	ERROR_EXPECT,
	ERROR_ALLOC,
} error_t;

#define no_error ERROR_NONE

#define or_return(e, error) \
	do { \
		error = (e); \
		if (error != ERROR_NONE) return error; \
	} while (0)

struct node {
	size_t index;
	struct token token;
};

struct parser {
	struct lexer lexer;
	struct token token;
	struct arena arena;
	size_t index;
	size_t parent;
};

error_t parse(struct parser *);

error_t load(struct parser *);
error_t commit(struct parser *);
error_t derive(struct parser *);

error_t parse_scope(struct parser *);
error_t parse_sequence(struct parser *);
error_t parse_expression(struct parser *, char8_t);
error_t parse_statement(struct parser *);
error_t parse_pseudo(struct parser *);
error_t parse_if(struct parser *);

#endif // !PARSER_H
