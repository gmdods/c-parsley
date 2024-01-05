CC = gcc
CFLAGS = -Wall -Wpedantic -Wextra \
	 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function \
	 -march=native -std=c11

FILES =  ../lexer.c
DEPS =  ../macros.h ../glyph.h ../lexer.h unittest.c

test: test.out
.PHONY: test

test.out: test.c $(DEPS)
	$(CC) $(CFLAGS) -Og -g $< $(FILES) -o $@ && ./$@

clean:
	rm *.out
