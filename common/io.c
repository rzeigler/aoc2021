#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/io.h"

// Total length of the buffer defined as buffer_end - buffer
#define BUFFERED_READER_TOTAL_LEN(buf) (buf->buffer_end - buf->buffer)
// Total length of the active content area defined as buffer_high_mark -
// buffer_low_mark
#define BUFFERED_READER_MARK_LEN(buf)                                          \
    (buf->buffer_high_mark - buf->buffer_low_mark)
// Total capacity for reading data defined as buffer_end - buffer_high_mark
#define BUFFERED_READER_AVAIL(buf) (buf->buffer_end - buf->buffer_high_mark)

const size_t INITIAL_BUFFER_SZ = 16;
const size_t MAX_BUFFER_SZ = 1024 * 1024;

size_t buffered_reader_ensure_read_capacity(buffered_reader *in_buf);

size_t buffered_reader_read_block(buffered_reader *in_buf, size_t capacity);

char *buffered_reader_needle_search(buffered_reader *in_buf);

char *buffered_reader_needle_extract(buffered_reader *in_buf, char *needle);

// Initialize a new buffered_reader by
// Assumes that buffer_by is NULL terminated
// On success tokens ownership of file
buffered_reader *buffered_reader_create(FILE *file, char *buffer_by) {
    buffered_reader *result = malloc(sizeof(buffered_reader));
    if (result == NULL) {
        return NULL;
    }
    if (!buffered_reader_init(result, file, buffer_by)) {
        free(result);
        return NULL;
    }
    return result;
}

int buffered_reader_init(buffered_reader *in_buf, FILE *file, char *buffer_by) {
    // Do the invalid input check
    if (file == NULL || buffer_by == NULL) {
        return 0;
    }

    memset(in_buf, 0, sizeof(buffered_reader));

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

void buffered_reader_release(buffered_reader *in_buf) {
    if (in_buf) {
        buffered_reader_uninit(in_buf);
        free(in_buf);
    }
}

void buffered_reader_uninit(buffered_reader *in_buf) {
    fclose(in_buf->file);
    free(in_buf->buffer);
    free(in_buf->buffer_by);
}

char *buffered_reader_read(buffered_reader *in_buf) {
    char *needle = buffered_reader_needle_search(in_buf);
    int can_read = !feof(in_buf->file) && !ferror(in_buf->file);
    size_t read_count = 0;
    while (!needle && can_read) {
        size_t cap = buffered_reader_ensure_read_capacity(in_buf);
        if (!cap) {
            return NULL;
        }
        read_count = buffered_reader_read_block(in_buf, cap);
        if (read_count < cap) {
            can_read = 0;
            if (ferror(in_buf->file)) {
                fputs("ERROR READING FILE", stderr);
            }
        }
        needle = buffered_reader_needle_search(in_buf);
    }
    if (needle) {
        return buffered_reader_needle_extract(in_buf, needle);
    }
    if (in_buf->buffer_low_mark < in_buf->buffer_high_mark) {
        char *result =
            strndup(in_buf->buffer_low_mark,
                    in_buf->buffer_high_mark - in_buf->buffer_low_mark);
        in_buf->buffer_low_mark = in_buf->buffer_high_mark;
        return result;
    }
    return NULL;
}

size_t buffered_reader_ensure_read_capacity(buffered_reader *in_buf) {
    size_t current_len = BUFFERED_READER_TOTAL_LEN(in_buf);
    size_t current_mark_len = BUFFERED_READER_MARK_LEN(in_buf);

    if (in_buf->buffer_high_mark < in_buf->buffer_end) {
        return BUFFERED_READER_AVAIL(in_buf);
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
    return BUFFERED_READER_AVAIL(in_buf);
}

size_t buffered_reader_read_block(buffered_reader *in_buf, size_t cap) {
    size_t read = fread(in_buf->buffer_high_mark, 1, cap, in_buf->file);
    in_buf->buffer_high_mark += read;
    return read;
}

char *buffered_reader_needle_search(buffered_reader *in_buf) {
    return common_needle_search(in_buf->buffer_low_mark,
                                in_buf->buffer_high_mark, in_buf->buffer_by);
}

char *buffered_reader_needle_extract(buffered_reader *in_buf, char *needle) {
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