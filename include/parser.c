#include "parser.h"
#include "lexer.h"
#include "macros.h"

error_t load(struct parser * parser) {
	struct token token = lex(&parser->lexer);
	parser->token = token;
	if (token.type == TOKEN_BAD) return (error_t){ERROR_BAD};
	return no_error;
}

error_t commit(struct parser * parser) {
	error_t error = no_error;
	index_t index = parser->index++;
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
	return no_error;
}

error_t expect(struct parser * parser, char8_t expected) {
	if (parser->token.type != expected) return (error_t){ERROR_EXPECT};
	return no_error;
}

error_t parse_expression(struct parser * parser, char8_t precedence) {
	error_t error = no_error;
	index_t parent = parser->parent;

	switch ((char8_t) parser->token.type) {
	case '(':
		or_return(derive(parser), error);
		or_return(parse_expression(parser, 0), error);
		or_return(expect(parser, ')'), error);
		or_return(load(parser), error);
		break_case;

	case '+' or_ case '-':					    // Sign
	case '!' or_ case '~':					    // Nots
	case '&' or_ case '*':					    // Address
	case '\'' or_ case '"' or_ case '`' or_ case TOKEN_DECIMAL: // Literals
	case TOKEN_LABEL: or_return(commit(parser), error); break_case;

	default: return (error_t){ERROR_EXPECT};
	case TOKEN_EOF: return ERROR_EOF;
	}

	for (char8_t level = 0; (level = 0, 1);)
		switch ((char8_t) parser->token.type) {
		case '(':
			or_return(derive(parser), error);
			do {
				or_return(parse_expression(parser, 0), error);
			} while (parser->token.type == ',');
			or_return(expect(parser, ')'), error);
			or_return(load(parser), error);
			break_case;

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
			if (precedence < level) {
				or_return(derive(parser), error);
				or_return(parse_expression(parser, level),
					  error);
				break_case;
			}
			fallthrough;

		case tokenof(KEYWORD_THEN):
		case tokenof(KEYWORD_ELSE):
		case ']' or_ case '}' or_ case ')':
		case ';' or_ case ',': parser->parent = parent; return no_error;

		case '!' or_ case '~': return (error_t){ERROR_EXPECT};

		case TOKEN_LABEL:
		default: or_return(commit(parser), error); break_case;
		case TOKEN_EOF: return ERROR_EOF;
		}
}

error_t parse_statement(struct parser * parser) {
	error_t error = no_error;
	index_t parent = parser->parent;
	if (parser->token.type == tokenof(KEYWORD_RETURN)) {
		or_return(derive(parser), error);
		or_return(parse_expression(parser, 0), error);
	} else if (parser->token.type == tokenof(KEYWORD_BREAK)) {
		or_return(derive(parser), error);
	} else if (parser->token.type == tokenof(KEYWORD_LET)) {
		or_return(derive(parser), error);
		or_return(expect(parser, TOKEN_LABEL), error);
		or_return(commit(parser), error);
		or_return(expect(parser, '='), error);
		or_return(commit(parser), error);
		or_return(parse_expression(parser, 0), error);
	}
	or_return(expect(parser, ';'), error);
	or_return(load(parser), error);
	parser->parent = parent;
	return no_error;
}

error_t parse_pseudo(struct parser * parser) {
	error_t error = no_error;
	or_return(derive(parser), error);
	index_t parent = parser->parent;

	or_return(expect(parser, '('), error);
	or_return(derive(parser), error);
	or_return(parse_expression(parser, 0), error);
	or_return(expect(parser, ')'), error);
	or_return(load(parser), error);
	parser->parent = parent;
	return no_error;
}

error_t parse_if(struct parser * parser) {
	error_t error = no_error;
	index_t parent = parser->parent;
	or_return(parse_pseudo(parser), error);
	index_t index = parser->parent;
	if (parser->token.type == '{') {
		or_return(parse_scope(parser), error);
		parser->parent = index;
		if (parser->token.type == tokenof(KEYWORD_ELSE)) {
			or_return(derive(parser), error);
			or_return(parse_scope(parser), error);
		}
	} else if (parser->token.type == tokenof(KEYWORD_THEN)) {
		or_return(derive(parser), error);
		or_return(parse_expression(parser, 0), error);
		parser->parent = index;
		if (parser->token.type == tokenof(KEYWORD_ELSE)) {
			or_return(derive(parser), error);
			or_return(parse_expression(parser, 0), error);
		}
	} else
		return (error_t){ERROR_EXPECT};
	parser->parent = parent;
	return no_error;
}

error_t parse_scope(struct parser * parser) {
	error_t error = no_error;
	index_t parent = parser->parent;
	or_return(expect(parser, '{'), error);
	or_return(derive(parser), error);
	or_return(parse_sequence(parser), error);
	or_return(expect(parser, '}'), error);
	or_return(load(parser), error);
	parser->parent = parent;
	return no_error;
}

error_t parse_sequence(struct parser * parser) {
	error_t error = no_error;
	for (index_t parent = parser->index; (parser->parent = parent, 1);)
		switch ((char8_t) parser->token.type) {
		case tokenof(KEYWORD_WHILE):
			or_return(parse_pseudo(parser), error);
			or_return(parse_scope(parser), error);
			break_case;
		case tokenof(KEYWORD_IF):
			or_return(parse_if(parser), error);
			break_case;
		case tokenof(KEYWORD_RETURN):
		case tokenof(KEYWORD_BREAK):
		case tokenof(KEYWORD_LET):
			or_return(parse_statement(parser), error);
			break_case;
		case tokenof(KEYWORD_LOOP):
			or_return(derive(parser), error);
			or_return(parse_scope(parser), error);
			break_case;
		case TOKEN_LABEL:
			or_return(parse_expression(parser, 0), error);
			break_case;
		case '}': return no_error;
		case TOKEN_EOF: return ERROR_EOF;
		default: return (error_t){ERROR_BAD};
		}
}

error_t parse(struct parser * parser) {
	error_t error = no_error;
	or_return(load(parser), error);
	return parse_sequence(parser);
}
