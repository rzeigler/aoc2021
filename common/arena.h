#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>

typedef struct zone {
    void *region;
    void *offset;
    zone *next;
} zone;

typedef struct arena {
    zone head;
    zone *current_zone;
    size_t region_len;

} arena;

int arena_init(arena *a, size_t region_len);

void arena_destroy(arena *a);

void *arena_alloc(arena *a, size_t size);

#endif
