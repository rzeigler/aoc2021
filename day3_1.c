#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/defs.h"
#include "common/io.h"

unsigned long *read_input(buffered_reader *in_buf, size_t *result) {
    size_t cap = 1024;
    size_t len = 0;
    unsigned long *vec = malloc(cap * sizeof(long));
    unsigned long *resize_vec = NULL;
    if (!vec) {
        return NULL;
    }

    char *line = buffered_reader_read(in_buf);
    while (line) {
        if (len == cap) {
            cap = cap * 2;
            resize_vec = realloc(vec, cap);
            if (!resize_vec) {
                free(vec);
                return NULL;
            }
            vec = resize_vec;
        }
        *(vec + len++) = strtoul(line, NULL, 2);

        free(line);
        line = buffered_reader_read(in_buf);
    }
    *result = len;
    return vec;
}

unsigned long high_bit(unsigned long n) {
    unsigned long bit = 31;
    unsigned long mask = 1 << bit;
    while ((mask & n) != mask && bit > 0) {
        bit -= 1;
        mask = 1 << bit;
    }
    return bit;
}

int has_bit(unsigned long n, unsigned long bit) {
    unsigned long mask = 1 << bit;
    return (n & mask) == mask;
}

unsigned long gamma_rate(unsigned long *numbers, size_t num_len,
                         unsigned long high_bit) {
    unsigned long result = 0;
    for (unsigned int bit = 0; bit <= high_bit; bit++) {
        unsigned int count = 0;
        for (size_t i = 0; i < num_len; i++) {
            if (has_bit(numbers[i], bit)) {
                count++;
            }
        }
        if (count > num_len / 2) {
            result |= 1 << bit;
        }
    }
    return result;
}

unsigned long epsilon_rate(unsigned long *numbers, size_t num_len,
                           unsigned int high_bit) {
    unsigned long result = 0;
    for (unsigned int bit = 0; bit <= high_bit; bit++) {
        unsigned int count = 0;
        for (size_t i = 0; i < num_len; i++) {
            if (has_bit(numbers[i], bit)) {
                count++;
            }
        }
        if (count < num_len / 2) {
            result |= 1 << bit;
        }
    }
    return result;
}

int main() {
    buffered_reader in_buf;
    buffered_reader_init(&in_buf, stdin, "\n");

    size_t len;
    unsigned long *vec = read_input(&in_buf, &len);

    buffered_reader_uninit(&in_buf);
    if (!vec) {
        fputs("EIIK!", stderr);
    }

    unsigned long h_bit = 0;
    for (size_t i = 0; i < len; i++) {
        h_bit = MAX(h_bit, high_bit(vec[i]));
    }

    unsigned long gamma = gamma_rate(vec, len, h_bit);
    unsigned long epsilon = epsilon_rate(vec, len, h_bit);

    free(vec);

    printf("%ld\n", gamma * epsilon);
}
