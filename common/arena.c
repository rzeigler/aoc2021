#include <stdio.h>
#include <stdlib.h>

#include "arena.h"

int arena_init(arena *a, size_t region_len) {
    void *region = malloc(region_len);
    if (!region) {
        return 0;
    }
    a->region_len = region_len;
    a->head.region = region;
    a->head.offset = region;
    a->head.next = NULL;
    return 1;
}

void arena_destroy(arena *a) {
    zone *next_zone = NULL;
    zone *zone = &a->head;
    while (zone) {
        next_zone = zone->next;
        free(zone);
        zone = next_zone;
    }
}

void *arena_alloc(arena *a, size_t size) {
    if (size > a->region_len) {
        fputs("CANNOT ALLOCATE THAT MUCH", stderr);
        return NULL;
    }
    if (a->current_zone->offset + size <=
        a->current_zone->region + a->region_len) {
        void *output = a->current_zone->offset;
        a->current_zone->offset += size;
        return output;
    }
    a->current_zone->next = malloc(a->region_len);
    a->current_zone = a->current_zone->next;
    return arena_alloc(a, size);
}
