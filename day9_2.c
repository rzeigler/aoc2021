#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/io.h"

void free_input(char **input, size_t input_len) {
    for (size_t i = 0; i < input_len; i++) {
        free(input[i]);
    }
    free(input);
}

char **read_input(int *input_len, int *width) {
    buffered_reader reader;
    buffered_reader_init(&reader, stdin, "\n");

    int cap = 100;
    *input_len = 0;
    *width = 0;

    char **y_axis = malloc(sizeof(char *) * cap);
    if (!y_axis) {
        return NULL;
    }

    char *line = buffered_reader_read(&reader);

    if (line) {
        *width = strlen(line);
    }

    while (line) {
        if (*input_len == cap) {
            cap *= 2;
            char **new_y_axis = realloc(y_axis, sizeof(char *) * cap);
            if (!new_y_axis) {
                goto on_error;
            }
            y_axis = new_y_axis;
        }
        y_axis[*input_len] = line;
        (*input_len)++;
        line = buffered_reader_read(&reader);
    }

    buffered_reader_uninit(&reader);

    return y_axis;

on_error:
    free_input(y_axis, *input_len);
    return NULL;
}

int is_low(char **input, int input_len, int width, int y, int x) {
    if (y > 0) {
        if (x > 0) {
            if (input[y - 1][x - 1] <= input[y][x]) {
                return 0;
            }
        }
        if (input[y - 1][x] <= input[y][x]) {
            return 0;
        }
        if (x < width - 1) {
            if (input[y - 1][x + 1] <= input[y][x]) {
                return 0;
            }
        }
    }
    if (x > 0) {
        if (input[y][x - 1] <= input[y][x]) {
            return 0;
        }
    }
    if (x < width - 1) {
        if (input[y][x + 1] <= input[y][x]) {
            return 0;
        }
    }
    if (y < input_len - 1) {
        if (x > 0) {
            if (input[y + 1][x - 1] <= input[y][x]) {
                return 0;
            }
        }
        if (input[y + 1][x] <= input[y][x]) {
            return 0;
        }
        if (x < width - 1) {
            if (input[y + 1][x + 1] <= input[y][x]) {
                return 0;
            }
        }
    }

    return 1;
}

typedef struct point {
    int x;
    int y;
} point;

int main() {
    int input_len, width;
    char **input = read_input(&input_len, &width);
    if (!input) {
        fputs("SAD\n", stderr);
        return 1;
    }

    unsigned int risk_level = 0;
    for (int y = 0; y < input_len; y++) {
        for (int x = 0; x < width; x++) {
            if (is_low(input, input_len, width, y, x)) {
                risk_level += CHAR_DIGIT(input[y][x]) + 1;
            }
        }
    }

    free_input(input, input_len);

    printf("%u\n", risk_level);
    return 0;
}
