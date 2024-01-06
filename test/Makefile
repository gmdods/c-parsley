CC = gcc
CFLAGS = -Wall -Wpedantic -Wextra \
	 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function \
	 -march=native -std=c11

FILES = ../include/lexer.c ../include/parser.c
DEPS  = ../include/macros.h ../include/defs.h ../include/keyword.h \
	../include/memory.h ../include/lexer.h ../include/parser.h \
	unittest.c

test: test.out
.PHONY: test

test.out: test.c $(DEPS) $(FILES)
	$(CC) $(CFLAGS) -Og -g $< $(FILES) -o $@ && ./$@

asm: parser.S lexer.S
.PHONY: asm

lexer.S: ../include/lexer.c ../include/lexer.h
	$(CC) $(CFLAGS) -O3 -S $< -o $@

parser.S: ../include/parser.c ../include/parser.h
	$(CC) $(CFLAGS) -O3 -S $< -o $@

clean:
	rm *.out

tidy:
	rm *.S
