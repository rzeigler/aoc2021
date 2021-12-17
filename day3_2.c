#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"

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

int not_has_bit(unsigned long n, unsigned long bit) { return !has_bit(n, bit); }

size_t count_where(unsigned long *vec, size_t vec_len, unsigned long ctx,
                   int (*predicate)(unsigned long, unsigned long)) {
    size_t count = 0;
    for (size_t i = 0; i < vec_len; i++) {
        if (predicate(*(vec + i), ctx)) {
            count += 1;
        }
    }
    return count;
}

void retain_where(unsigned long *vec, size_t *vec_len, unsigned long ctx,
                  int (*predicate)(unsigned long, unsigned long)) {
    for (size_t i = 0; i < *vec_len; i++) {
        if (!predicate(*(vec + i), ctx)) {
            if (i < *vec_len - 1) {
                vec[i] = vec[*vec_len - 1];
            }
            (*vec_len)--;
        }
    }
}

unsigned long o2_rating(unsigned long *vec, size_t *vec_len,
                        unsigned int high_bit) {
    size_t count;
    size_t off_count;
    for (unsigned int bit = high_bit; bit <= high_bit || *vec_len > 1; bit--) {
        count = count_where(vec, *vec_len, bit, has_bit);
        off_count = *vec_len - count;
        if (count >= off_count) {
            retain_where(vec, vec_len, bit, has_bit);
        } else {
            retain_where(vec, vec_len, bit, not_has_bit);
        }
    }
    return vec[0];
}

unsigned long co2_rating(unsigned long *vec, size_t *vec_len,
                         unsigned int high_bit) {
    size_t count;
    size_t off_count;
    for (unsigned int bit = high_bit; bit <= high_bit || *vec_len > 1; bit--) {
        count = count_where(vec, *vec_len, bit, has_bit);
        off_count = *vec_len - count;
        if (count >= off_count) {
            retain_where(vec, vec_len, bit, has_bit);
        } else {
            retain_where(vec, vec_len, bit, not_has_bit);
        }
    }
    return vec[0];
}

int main() {
    buffered_reader in_buf;
    buffered_reader_init(&in_buf, stdin, "\n");

    size_t o2_len;
    unsigned long *o2_vec = read_input(&in_buf, &o2_len);

    if (!o2_vec) {
        fputs("EIIK!", stderr);
        return 1;
    }

    buffered_reader_uninit(&in_buf);

    size_t co2_len = o2_len;
    unsigned long *co2_vec = malloc(o2_len * sizeof(unsigned long));
    if (!co2_vec) {
        fputs("Duplicate failure", stderr);
        free(o2_vec);
        return 1;
    }
    memcpy(co2_vec, o2_vec, o2_len * sizeof(unsigned long));

    unsigned long h_bit = 0;
    for (size_t i = 0; i < o2_len; i++) {
        h_bit = MAX(h_bit, high_bit(o2_vec[i]));
    }

    unsigned long oxygen = o2_rating(o2_vec, &o2_len, h_bit);
    unsigned long co2 = co2_rating(co2_vec, &co2_len, h_bit);

    free(o2_vec);
    free(co2_vec);

    printf("%lu\n", oxygen * co2);
}
