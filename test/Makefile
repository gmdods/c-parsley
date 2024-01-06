CC = gcc
CFLAGS = -Wall -Wpedantic -Wextra \
	 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function \
	 -march=native -std=c11

FILES =  ../lexer.c ../parser.c
DEPS =  ../macros.h ../glyph.h ../keyword.h ../memory.h ../lexer.h ../parser.h unittest.c

test: test.out
.PHONY: test

test.out: test.c $(DEPS) $(FILES)
	$(CC) $(CFLAGS) -Og -g $< $(FILES) -o $@ && ./$@

asm: parser.S lexer.S
.PHONY: asm

lexer.S: ../lexer.c ../lexer.h
	$(CC) $(CFLAGS) -O3 -S $< -o $@

parser.S: ../parser.c ../parser.h
	$(CC) $(CFLAGS) -O3 -S $< -o $@

clean:
	rm *.out *.S
