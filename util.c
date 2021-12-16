#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

// Total length of the buffer defined as buffer_end - buffer
#define INPUT_BUFFER_TOTAL_LEN(buf) (buf->buffer_end - buf->buffer)
// Total length of the active content area defined as buffer_high_mark - buffer_low_mark
#define INPUT_BUFFER_MARK_LEN(buf)                                             \
    (buf->buffer_high_mark - buf->buffer_low_mark)
// Total capacity for reading data defined as buffer_end - buffer_high_mark
#define INPUT_BUFFER_AVAIL(buf) (buf->buffer_end - buf->buffer_high_mark)

const size_t INITIAL_BUFFER_SZ = 16;
const size_t MAX_BUFFER_SZ = 1024 * 1024;

size_t input_buffer_ensure_read_capacity(input_buffer *in_buf);

size_t input_buffer_read_block(input_buffer *in_buf, size_t capacity);

char *input_buffer_needle_search(input_buffer *in_buf);

char *input_buffer_needle_extract(input_buffer *in_buf, char *needle);

char *common_needle_search(char *initial_pos, char *end_pos, char *needle) {
    size_t needle_len = strlen(needle);
    for (char *start_pos = initial_pos;
         start_pos <= end_pos - needle_len;
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

// Initialize a new input_buffer by
// Assumes that buffer_by is NULL terminated
// On success tokens ownership of file
input_buffer *input_buffer_create(FILE *file, char *buffer_by) {
    input_buffer *result = malloc(sizeof(input_buffer));
    if (result == NULL) {
        return NULL;
    }
    if (!input_buffer_init(result, file, buffer_by)) {
        free(result);
        return NULL;
    }
    return result;
}

int input_buffer_init(input_buffer *in_buf, FILE *file, char *buffer_by) {
    // Do the invalid input check
    if (file == NULL || buffer_by == NULL) {
        return 0;
    }

    memset(in_buf, 0, sizeof(input_buffer));

    in_buf->file = file;
    in_buf->buffer = malloc(INITIAL_BUFFER_SZ);
    if (in_buf->buffer == NULL) {
        goto on_error;
    }
    in_buf->buffer_low_mark = in_buf->buffer;
    in_buf->buffer_high_mark = in_buf->buffer;
    in_buf->buffer_end = in_buf->buffer + INITIAL_BUFFER_SZ;

    size_t buffer_by_len = strlen(buffer_by);

    in_buf->buffer_by = malloc(buffer_by_len + 1);
    if (in_buf->buffer_by == NULL) {
        goto on_error;
    }
    strcpy(in_buf->buffer_by, buffer_by);

    return 1;

on_error:
    // In this case we don't fail
    free(in_buf->buffer);
    free(in_buf->buffer_by);
    return 0;
}

void input_buffer_release(input_buffer *in_buf) {
    if (in_buf) {
        input_buffer_uninit(in_buf);
        free(in_buf);
    }
}

void input_buffer_uninit(input_buffer *in_buf) {
    fclose(in_buf->file);
    free(in_buf->buffer);
    free(in_buf->buffer_by);
}

char *input_buffer_read(input_buffer *in_buf) {
    char *needle = input_buffer_needle_search(in_buf);
    int can_read = !feof(in_buf->file) && !ferror(in_buf->file);
    size_t read_count = 0;
    while (!needle && can_read) {
        size_t cap = input_buffer_ensure_read_capacity(in_buf);
        if (!cap) {
            return NULL;
        }
        read_count = input_buffer_read_block(in_buf, cap);
        if (read_count < cap) {
            can_read = 0;
            if (ferror(in_buf->file)) {
                fputs("ERROR READING FILE", stderr);
            }
        }
        needle = input_buffer_needle_search(in_buf);
    }
    if (needle) {
        return input_buffer_needle_extract(in_buf, needle);
    }
    return NULL;
}

size_t input_buffer_ensure_read_capacity(input_buffer *in_buf) {
    size_t current_len = INPUT_BUFFER_TOTAL_LEN(in_buf);
    size_t current_mark_len = INPUT_BUFFER_MARK_LEN(in_buf);

    if (in_buf->buffer_high_mark < in_buf->buffer_end) {
        return INPUT_BUFFER_AVAIL(in_buf);
    }
    // If we the buffer_low mark is past the halfway mark we can copy the high
    // half to the low half In order to free up space for reading
    if (in_buf->buffer_low_mark >
        in_buf->buffer_low_mark + current_len / 2 + 1) {
        memcpy(in_buf->buffer, in_buf->buffer_low_mark, current_mark_len);
        in_buf->buffer_low_mark = in_buf->buffer;
        in_buf->buffer_high_mark = in_buf->buffer_low_mark + current_mark_len;
    } else if ((size_t)(in_buf->buffer_end - in_buf->buffer) >= MAX_BUFFER_SZ) {
        // Not going to worry about propagating the error for now
        // In this case we just give up
        fputs("INPUT BUFFER TOO LARGE!", stderr);
        return 0;
    } else {
        size_t low_offset = in_buf->buffer_low_mark - in_buf->buffer;
        // In this case we can resize all the things
        char *realloc_buffer = realloc(in_buf->buffer, current_len * 2);
        if (realloc_buffer == NULL) {
            fputs("MEMORY ALLOC FAILURE", stderr);
            return 0;
        }
        in_buf->buffer = realloc_buffer;
        in_buf->buffer_end = in_buf->buffer + current_len * 2;
        in_buf->buffer_low_mark = in_buf->buffer + low_offset;
        in_buf->buffer_high_mark = in_buf->buffer_low_mark + current_mark_len;
    }
    return INPUT_BUFFER_AVAIL(in_buf);
}

size_t input_buffer_read_block(input_buffer *in_buf, size_t cap) {
    size_t read = fread(in_buf->buffer_high_mark, 1, cap, in_buf->file);
    in_buf->buffer_high_mark += read;
    return read;
}

char *input_buffer_needle_search(input_buffer *in_buf) {
    return common_needle_search(in_buf->buffer_low_mark, in_buf->buffer_high_mark, in_buf->buffer_by);
}

char *input_buffer_needle_extract(input_buffer *in_buf, char *needle) {
    size_t needle_len = strlen(in_buf->buffer_by);
    size_t found_len = needle - in_buf->buffer_low_mark;
    char *extract = strndup(in_buf->buffer_low_mark, found_len);
    if (!extract) {
        fputs("MEMORY ALLOC FAILURE", stderr);
        return NULL;
    }
    // move things forward
    in_buf->buffer_low_mark = needle + needle_len;
    return extract;
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
