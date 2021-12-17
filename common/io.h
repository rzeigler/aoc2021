#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#include "text.h"

typedef struct buffered_reader {
    FILE *file;
    char *buffer;
    char *buffer_low_mark;
    char *buffer_high_mark;
    char *buffer_end;
    char *buffer_by;
} buffered_reader;

buffered_reader *buffered_reader_create(FILE *file, char *buffer_by);

int buffered_reader_init(buffered_reader *in_buf, FILE *file, char *buffer_by);

void buffered_reader_release(buffered_reader *in_buf);

void buffered_reader_uninit(buffered_reader *in_buf);

char *buffered_reader_read(buffered_reader *buf);

#endif
