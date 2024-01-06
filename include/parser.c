#include "parser.h"
#include "keyword.h"
#include "lexer.h"
#include "macros.h"
#include "memory.h"

error_t load(struct parser * parser) {
	struct token token = lex(&parser->lexer);
	parser->token = token;
	if (token.type == TOKEN_BAD) return (error_t){ERROR_BAD};
	return no_error;
}

error_t commit(struct parser * parser) {
	size_t index = parser->index++;
	upalloc(&parser->arena, sizeof(struct node) * (1 + index));
	if (!parser->arena.ptr) return (error_t){ERROR_ALLOC};
	array_cast(struct node, parser->arena)[index] =
	    (struct node){parser->parent, parser->token};
	or_return(load(parser), error);
	return no_error;
}

error_t derive(struct parser * parser) {
	error_t error = no_error;
	or_return(commit(parser), error);
	parser->parent = parser->index;
	return error;
}

error_t expect(struct parser * parser, char8_t expected) {
	if (parser->token.type != expected) return (error_t){ERROR_EXPECT};
	return no_error;
}

error_t parse_expression(struct parser * parser, char8_t precedence) {
	char8_t level = 0;
	for (error_t error = no_error; (level = 0, 1);)
		switch ((char8_t) parser->token.type) {
		case ']' or_ case '}' or_ case ')' or_ case ';':
			return no_error;
		case '(':
			or_return(derive(parser), error);
			or_return(parse_expression(parser, 0), error);
			or_return(expect(parser, ')'), error);
			or_return(load(parser), error);
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
			or_return(derive(parser), error);
			or_return(parse_expression(parser, level), error);
			break_case;

		case '_':
			if (parser->token.type == tokenof(KEYWORD_ELSE))
				return no_error;
			fallthrough;
		default: or_return(commit(parser), error); break_case;
		case TOKEN_EOF: return ERROR_EOF;
		}
}

error_t parse_statement(struct parser * parser) {
	error_t error = no_error;
	if (parser->token.type == tokenof(KEYWORD_LET)) {
		or_return(derive(parser), error);
		size_t parent = parser->parent;
		or_return(expect(parser, TOKEN_LABEL), error);
		or_return(commit(parser), error);
		or_return(expect(parser, '='), error);
		or_return(commit(parser), error);
	}
	or_return(parse_expression(parser, 0), error);
	or_return(expect(parser, ';'), error);
	or_return(load(parser), error);
	return no_error;
}

error_t parse_if(struct parser * parser) {
	error_t error = no_error;
	or_return(derive(parser), error);
	size_t parent = parser->parent;
	or_return(expect(parser, '('), error);
	or_return(load(parser), error);
	or_return(parse_expression(parser, 0), error);
	or_return(expect(parser, ')'), error);
	parser->parent = parent;
	or_return(load(parser), error);
	if (parser->token.type == '{') {
		or_return(derive(parser), error);
		or_return(parse_scope(parser), error);
		or_return(expect(parser, '}'), error);
		or_return(load(parser), error);
	} else
		or_return(parse_expression(parser, 0), error);

	if (parser->token.type == tokenof(KEYWORD_ELSE)) {
		or_return(derive(parser), error);
		if (parser->token.type == '{') {
			or_return(derive(parser), error);
			or_return(parse_scope(parser), error);
			or_return(expect(parser, '}'), error);
			or_return(load(parser), error);
		} else
			return parse_expression(parser, 0);
	}
	return no_error;
}

error_t parse_scope(struct parser * parser) {
	error_t error = no_error;
	or_return(load(parser), error);
	for (size_t parent = parser->index; (parser->parent = parent, 1);)
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
