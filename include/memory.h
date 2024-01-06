#ifndef MEMORY_H
#define MEMORY_H

#include "glyph.h"
#include <stdlib.h>

struct arena {
	char8_t * ptr;
	size_t bytes;
};

static inline struct arena alloc(size_t bytes) {
	char8_t * ptr = (char8_t *) calloc(bytes, 1);
	if (!ptr) return (struct arena){0};
	return (struct arena){.ptr = ptr, .bytes = bytes};
}

static inline void upalloc(struct arena * arena, size_t bytes) {
	if (!arena) return;
	if (bytes == 0) return;
	char8_t * ptr = (arena->ptr) ? (char8_t *) realloc(arena->ptr, bytes)
				     : (char8_t *) calloc(bytes >> 8, 64);
	if (!ptr)
		*arena = (struct arena){0};
	else
		*arena = (struct arena){.ptr = ptr, .bytes = bytes};
}

static inline void dealloc(struct arena * arena) {
	if (!arena) return;
	free(arena->ptr);
	*arena = (struct arena){0};
}

#endif // !MEMORY_H
