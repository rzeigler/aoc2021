#ifndef TEXT_H
#define TEXT_H

#include <stdlib.h>

char *common_needle_search(char *initial_pos, char *end_pos, char *needle);

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

#define CHAR_DIGIT(c) (((unsigned int)c) - 48)

#endif
