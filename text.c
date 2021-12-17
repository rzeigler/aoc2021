#include <stdlib.h>
#include <string.h>

#include "text.h"

char *common_needle_search(char *initial_pos, char *end_pos, char *needle) {
    size_t needle_len = strlen(needle);
    for (char *start_pos = initial_pos; start_pos <= end_pos - needle_len;
         start_pos += 1) {
        char *haystack_pos = start_pos;
        char *needle_pos = needle;
        while (*needle_pos != '\0' && *needle_pos == *haystack_pos) {
            needle_pos += 1;
            haystack_pos += 1;
        }
        if (*needle_pos == '\0') {
            return start_pos;
        }
    }
    return NULL;
}

tokenizer *tokenizer_create(char *source, char *split_by) {
    tokenizer *tokz = malloc(sizeof(tokenizer));
    if (!tokz) {
        return NULL;
    }
    if (!tokenizer_init(tokz, source, split_by)) {
        free(tokz);
        return NULL;
    }
    return tokz;
}

int tokenizer_init(tokenizer *tokz, char *source, char *split_by) {
    char *owned_split_by = strndup(split_by, strlen(split_by) + 1);
    if (!owned_split_by) {
        return 0;
    }
    tokz->split_by = owned_split_by;
    tokz->source = source;
    tokz->start = source;
    tokz->end = source + strlen(source);
    return 1;
}

void tokenizer_release(tokenizer *tokz) {
    if (tokz) {
        tokenizer_uninit(tokz);
        free(tokz);
    }
}

void tokenizer_uninit(tokenizer *tokz) {
    free(tokz->source);
    free(tokz->split_by);
}

char *tokenizer_next(tokenizer *tokz, size_t *len) {
    if (tokz->start == tokz->end) {
        *len = 0;
        return NULL;
    }
    char *start = tokz->start;
    char *needle = common_needle_search(tokz->start, tokz->end, tokz->split_by);
    if (!needle) {
        // Emit the final token
        *len = tokz->end - tokz->start;
        tokz->start = tokz->end;
        return start;
    }
    size_t needle_len = strlen(tokz->split_by);
    *len = needle - tokz->start;
    tokz->start = needle + needle_len;
    return start;
}

unsigned int str_atoui(str *s) {
    if (!s->str || s->str_len == 0) {
        return 0;
    }
    int count = 0;
    for (size_t i = 0; i < s->str_len; i++) {
        count *= 10;
        switch (s->str[i]) {
        case '1':
            count += 1;
            break;
        case '2':
            count += 2;
            break;
        case '3':
            count += 3;
            break;
        case '4':
            count += 4;
            break;
        case '5':
            count += 5;
            break;
        case '6':
            count += 6;
            break;
        case '7':
            count += 7;
            break;
        case '8':
            count += 8;
            break;
        case '9':
            count += 9;
            break;
        case '0':
            break;
        default:
            return 0;
        }
    }
    return count;
}
