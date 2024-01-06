#include "parser.h"
#include "keyword.h"
#include "lexer.h"
#include "macros.h"
#include "memory.h"

enum token_type load(struct parser * parser) {
	struct token token = lex(&parser->lexer);
	parser->token = token;
	return token.type;
}

error_t commit(struct parser * parser) {
	size_t index = parser->index++;
	if (parser->arena.bytes < sizeof(struct node) * (1 + index)) {
		upalloc(&parser->arena, 2 * parser->arena.bytes);
	}
	if (!parser->arena.ptr) return (error_t){ERROR_ALLOC};
	((struct node *) parser->arena.ptr)[index] =
	    (struct node){parser->token, 0};
	load(parser);
	return no_error;
}

error_t expect(struct parser * parser, char8_t expected) {
	if (parser->token.type != expected) return (error_t){ERROR_EXPECT};
	return no_error;
}

error_t parse_expression(struct parser * parser) {
	// TODO: Postfix
	int paren = 0;
	for (error_t error = no_error;;)
		switch ((char8_t) parser->token.type) {
		case '}' or_ case ';': return no_error;
		case '(': ++paren, load(parser); break_case;
		case ')':
			if (--paren < 0) return no_error;
			load(parser);
			break_case;
		case '_':
			if (keyword(parser->token, KEYWORD_ELSE))
				return no_error;
			fallthrough;
		default: commit(parser); break_case;
		case TOKEN_EOF: return ERROR_EOF;
		}
}

error_t parse_statement(struct parser * parser) {
	error_t error = no_error;
	if (keyword(parser->token, KEYWORD_LET)) {
		parser->token.type = TOKEN_ASCII + KEYWORD_LET;
		commit(parser);
		or_return(expect(parser, TOKEN_LABEL), error);
		commit(parser);
		or_return(expect(parser, '='), error);
		commit(parser);
	}
	or_return(parse_expression(parser), error);
	or_return(expect(parser, ';'), error);
	load(parser);
	return no_error;
}

error_t parse_if(struct parser * parser) {
	error_t error = no_error;
	or_return(expect(parser, '('), error);
	load(parser);
	or_return(parse_expression(parser), error);
	or_return(expect(parser, ')'), error);
	if (load(parser) == '{') {
		or_return(parse_scope(parser), error);
		or_return(expect(parser, '}'), error);
		load(parser);
	} else
		or_return(parse_expression(parser), error);

	if (keyword(parser->token, KEYWORD_ELSE)) {
		parser->token.type = TOKEN_ASCII + KEYWORD_ELSE;
		commit(parser);
		if (parser->token.type == '{') {
			or_return(parse_scope(parser), error);
			or_return(expect(parser, '}'), error);
			load(parser);
		} else
			return parse_expression(parser);
	}
	return no_error;
}

error_t parse_scope(struct parser * parser) {
	load(parser);
	for (error_t error = no_error;;)
		switch ((char8_t) parser->token.type) {
		case TOKEN_LABEL:
			if (keyword(parser->token, KEYWORD_LET)) {
				or_return(parse_statement(parser), error);
			} else if (keyword(parser->token, KEYWORD_IF)) {
				parser->token.type = TOKEN_ASCII + KEYWORD_IF;
				commit(parser);
				or_return(parse_if(parser), error);
			} else
				or_return(parse_expression(parser), error);
			break_case;
		case '}': return no_error;
		case TOKEN_EOF: return ERROR_EOF;
		default: return (error_t){ERROR_BAD};
		}
}

error_t parse(struct parser * parser) { return parse_scope(parser); }
