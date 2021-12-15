#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
typedef struct input_buffer {
    FILE *file;
    char *buffer;
    char *buffer_low_mark;
    char *buffer_high_mark;
    char *buffer_end;
    char *buffer_by;
} input_buffer;

input_buffer *input_buffer_create(FILE *file, char *buffer_by);

int input_buffer_init(input_buffer *in_buf, FILE *file, char *buffer_by);

void input_buffer_release(input_buffer *in_buf);

void input_buffer_uninit(input_buffer *in_buf);

char *input_buffer_read(input_buffer *buf);

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

int lazy_str_atoi(str *s);

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#endif
