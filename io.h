#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
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

typedef struct tokenizer {
    char *source;
    char *start;
    char *end;
    char *split_by;
} tokenizer;

// assumes ownership of source (needs free), does not take ownership of split_by
tokenizer *tokenizer_create(char *source, char *split_by);

int tokenizer_init(tokenizer *tokz, char *source, char *split_by);

void tokenizer_release(tokenizer *tokz);

void tokenizer_uninit(tokenizer *tokz);

// Pointer to the next occurrence in tokz along with length
char *tokenizer_next(tokenizer *tokz, size_t *len);

typedef struct str {
    char *str;
    size_t str_len;
} str;

unsigned int str_atoui(str *s);

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define CHAR_DIGIT(c) (((unsigned int)c) - 48)

#endif
