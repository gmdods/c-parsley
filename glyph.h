#ifndef GLYPH_H
#define GLYPH_H

#include <ctype.h>
#include <stdbool.h>
#include <uchar.h>

typedef unsigned char char8_t;
enum ascii_utf8 { ASCII_LIMIT = 0x7f, UTF8_INIT = 0xc0 };

inline bool is_letter(char8_t c) { return isalpha(c); }
inline bool is_digit(char8_t c) { return isdigit(c); }
inline bool is_label(char8_t c) { return isalpha(c) | (c == '_'); }
inline bool is_ident(char8_t c) { return is_label(c) | isdigit(c); }
inline bool is_point(char8_t c) {
	return (c == ';') | (c == ':') | (c == '.') | (c == ',');
}
inline bool is_open(char8_t c) { return (c == '(') | (c == '[') | (c == '{'); }
inline bool is_close(char8_t c) { return (c == ')') | (c == ']') | (c == '}'); }
inline bool is_bracket(char8_t c) { return is_open(c) | is_close(c); }
inline bool is_mark(char8_t c) { return is_point(c) | is_bracket(c); }

#endif // !GLYPH_H
