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
	    (struct node){parser->parent, parser->token};
	load(parser);
	return no_error;
}

error_t expect(struct parser * parser, char8_t expected) {
	if (parser->token.type != expected) return (error_t){ERROR_EXPECT};
	return no_error;
}

size_t establish(struct parser * parser) {
	commit(parser);
	return parser->parent = parser->index;
}

error_t parse_expression(struct parser * parser, char8_t precedence) {
	char8_t level = 0;
	for (error_t error = no_error; (level = 0, 1);)
		switch ((char8_t) parser->token.type) {
		case ']' or_ case '}' or_ case ')' or_ case ';':
			return no_error;
		case '(':
			establish(parser);
			or_return(parse_expression(parser, 0), error);
			if (parser->token.type != ')') return no_error;
			load(parser);
			break_case;
		case '!' or_ case '~': level += 1; fallthrough;

		case '*':
		case '/' or_ case '%': level += 1; fallthrough;
		case '+' or_ case '-': level += 1; fallthrough;
		case '<' or_ case '>':
		case digraph('<') or_ case digraph('>'):
			level += 1;
			fallthrough;
		case digraph('!') or_ case digraph('='):
			level += 1;
			fallthrough;
		case '&': level += 1; fallthrough;
		case '^': level += 1; fallthrough;
		case '|': level += 1; fallthrough;

		case '=':
			level += 1;
			if (precedence >= level) return no_error;
			establish(parser);
			or_return(parse_expression(parser, level), error);
			break_case;

		case '_':
			if (parser->token.type == tokenof(KEYWORD_ELSE))
				return no_error;
			fallthrough;
		default: commit(parser); break_case;
		case TOKEN_EOF: return ERROR_EOF;
		}
}

error_t parse_statement(struct parser * parser) {
	error_t error = no_error;
	if (parser->token.type == tokenof(KEYWORD_LET)) {
		size_t parent = establish(parser);
		or_return(expect(parser, TOKEN_LABEL), error);
		commit(parser);
		or_return(expect(parser, '='), error);
		commit(parser);
	}
	or_return(parse_expression(parser, 0), error);
	or_return(expect(parser, ';'), error);
	load(parser);
	return no_error;
}

error_t parse_if(struct parser * parser) {
	error_t error = no_error;
	size_t parent = establish(parser);
	or_return(expect(parser, '('), error);
	load(parser);
	or_return(parse_expression(parser, 0), error);
	or_return(expect(parser, ')'), error);
	parser->parent = parent;
	if (load(parser) == '{') {
		establish(parser);
		or_return(parse_scope(parser), error);
		or_return(expect(parser, '}'), error);
		load(parser);
	} else
		or_return(parse_expression(parser, 0), error);

	if (parser->token.type == tokenof(KEYWORD_ELSE)) {
		establish(parser);
		if (parser->token.type == '{') {
			establish(parser);
			or_return(parse_scope(parser), error);
			or_return(expect(parser, '}'), error);
			load(parser);
		} else
			return parse_expression(parser, 0);
	}
	return no_error;
}

error_t parse_scope(struct parser * parser) {
	load(parser);
	size_t parent = parser->index;
	for (error_t error = no_error; (parser->parent = parent, 1);)
		switch ((char8_t) parser->token.type) {
		case tokenof(KEYWORD_LET):
			or_return(parse_statement(parser), error);
			break_case;
		case tokenof(KEYWORD_IF):
			or_return(parse_if(parser), error);
			break_case;
		case TOKEN_LABEL:
			or_return(parse_expression(parser, 0), error);
			break_case;
		case '}': return no_error;
		case TOKEN_EOF: return ERROR_EOF;
		default: return (error_t){ERROR_BAD};
		}
}

error_t parse(struct parser * parser) { return parse_scope(parser); }