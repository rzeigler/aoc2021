#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

#define INPUT_BUFFER_TOTAL_LEN(buf) (buf->buffer_end - buf->buffer)
#define INPUT_BUFFER_MARK_LEN(buf)                                             \
    (buf->buffer_high_mark - buf->buffer_low_mark)
#define INPUT_BUFFER_AVAIL(buf) (buf->buffer_end - buf->buffer_high_mark)

const size_t INITIAL_BUFFER_SZ = 1024;
const size_t MAX_BUFFER_SZ = 1024 * 1024;

struct input_buffer {
    FILE *file;
    char *buffer;
    char *buffer_low_mark;
    char *buffer_high_mark;
    char *buffer_end;
    char *buffer_by;
};

size_t input_buffer_ensure_read_capacity(input_buffer *in_buf);

size_t input_buffer_read_block(input_buffer *in_buf, size_t capacity);

char *input_buffer_needle_search(input_buffer *in_buf);

char *input_buffer_needle_extract(input_buffer *in_buf, char *needle);

// Initialize a new input_buffer by
// Assumes that buffer_by is NULL terminated
// On success tokens ownership of file
input_buffer *input_buffer_create(FILE *file, char *buffer_by) {
    // Do the invalid input check
    if (file == NULL || buffer_by == NULL) {
        return NULL;
    }
    input_buffer *result = malloc(sizeof(input_buffer));
    if (result == NULL) {
        return result;
    }
    memset(result, 0, sizeof(input_buffer));

    result->file = file;
    result->buffer = malloc(INITIAL_BUFFER_SZ);
    if (result->buffer == NULL) {
        goto on_error;
    }
    result->buffer_low_mark = result->buffer;
    result->buffer_high_mark = result->buffer;
    result->buffer_end = result->buffer + INITIAL_BUFFER_SZ;

    result->buffer_by = malloc(strlen(buffer_by));
    if (result->buffer_by == NULL) {
        goto on_error;
    }
    strcpy(result->buffer_by, buffer_by);

    return result;

on_error:
    // In this case we don't fail
    free(result->buffer);
    free(result->buffer_by);
    return NULL;
}

void input_buffer_release(input_buffer *in_buf) {
    fclose(in_buf->file);
    free(in_buf->buffer);
    free(in_buf->buffer_by);
    free(in_buf);
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
    size_t needle_len = strlen(in_buf->buffer_by);
    for (char *start_pos = in_buf->buffer_low_mark;
         start_pos <= in_buf->buffer_high_mark - needle_len; start_pos += 1) {
        char *haystack_pos = start_pos;
        char *needle_pos = in_buf->buffer_by;
        while (*needle_pos != '\0' && *needle_pos == *haystack_pos) {
            needle_pos += 1;
            haystack_pos += 1;
        }
        // Advanced to the null terminator so start_pos is an occurrence of
        // needle
        if (*needle_pos == '\0') {
            return start_pos;
        }
    }
    return NULL;
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
