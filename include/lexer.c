#include "lexer.h"

struct token reserved(struct token token, const char8_t * ptr) {
	for (size_t key = 0; key != KEYWORD_NONE; ++key)
		if ((token.span == keywords[key].span) &&
		    (strncmp((const char *) ptr + token.init,
			     (const char *) keywords[key].ptr,
			     keywords[key].span) == 0)) {
			token.type = tokenof(key);
			return token;
		}
	return token;
}

struct token lex(struct lexer * lex) {
	struct token ret = {0};
	char8_t ch = '\0', quote = '\0';
	for (index_t it = lex->pos;;)
		switch (ch = lex->ptr[it]) {
		case TOKEN_COMMENT:
			while ((ch = lex->ptr[++it]) && (ch != '\n'))
				;
			fallthrough;
		case ' ' or_ case '\t' or_ case '\r' or_ case '\n':
			++it;
			break_case;
		case '"' or_ case '\'' or_ case '`':
			for (lex->pos = ++it, quote = ch;
			     (ch = lex->ptr[it]) && (ch != quote); ++it)
				;
			ret = (struct token){quote, lex->pos, it - lex->pos};
			lex->pos = ++it;
			return ret;

		case '0' or_ case '1' or_ case '2' or_ case '3' or_ case '4':
		case '5' or_ case '6' or_ case '7' or_ case '8' or_ case '9':
			for (lex->pos = it;
			     (ch = lex->ptr[++it]) && is_digit(ch);)
				;
			ret = (struct token){TOKEN_DECIMAL, lex->pos,
					     it - lex->pos};
			lex->pos = it;
			return ret;

		case 'a' or_ case 'b' or_ case 'c' or_ case 'd' or_ case 'e':
		case 'f' or_ case 'g' or_ case 'h' or_ case 'i' or_ case 'j':
		case 'k' or_ case 'l' or_ case 'm' or_ case 'n' or_ case 'o':
		case 'p' or_ case 'q' or_ case 'r' or_ case 's' or_ case 't':
		case 'u' or_ case 'v' or_ case 'w' or_ case 'x' or_ case 'y':
		case 'z':
		case 'A' or_ case 'B' or_ case 'C' or_ case 'D' or_ case 'E':
		case 'F' or_ case 'G' or_ case 'H' or_ case 'I' or_ case 'J':
		case 'K' or_ case 'L':
		case 'M' or_ case 'N' or_ case 'O' or_ case 'P' or_ case 'Q':
		case 'R' or_ case 'S' or_ case 'T' or_ case 'U' or_ case 'V':
		case 'W' or_ case 'X' or_ case 'Y' or_ case 'Z':
		case '_':
			for (lex->pos = it;
			     (ch = lex->ptr[++it]) && is_ident(ch);)
				;

			ret = reserved((struct token){TOKEN_LABEL, lex->pos,
						      it - lex->pos},
				       lex->ptr);
			lex->pos = it;
			return ret;

		case '<' or_ case '=' or_ case '>' or_ case '!':
			if (lex->ptr[it + 1] == '=') {
				ret = (struct token){digraph(ch), it, 2};
				lex->pos = (it += 2);
				return ret;
			}
			fallthrough;

		// For compound assignment move up
		case '+' or_ case '-':
		case '/' or_ case '%':
		case '*' or_ case '^':
		case '&' or_ case '|':
		case '~':

		case '[' or_ case '{' or_ case '(':
		case ']' or_ case '}' or_ case ')':
		case ':' or_ case '.' or_ case ';' or_ case ',':
			ret = (struct token){ch, it, 1};
			lex->pos = ++it;
			return ret;

		case '\0':
			ret = (struct token){TOKEN_EOF, it, 1};
			lex->pos = it;
			return ret;
		default:
			ret = (struct token){TOKEN_BAD, it, 1};
			lex->pos = it;
			return ret;
		}
}
