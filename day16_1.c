#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/io.h"

char decode_hex(char c) {
    if (c >= 48 && c <= 57) {
        return c - 48;
    }
    switch (c) {
    case 'A':
    case 'a':
        return 0xA;
    case 'B':
    case 'b':
        return 0xB;
    case 'C':
    case 'c':
        return 0xC;
    case 'D':
    case 'd':
        return 0xD;
    case 'E':
    case 'e':
        return 0xE;
    case 'F':
    case 'f':
        return 0xF;
    }
    fprintf(stderr, "UHOH: %c", c);
    return 0;
}

char *decode_input(char *input, size_t *output_len) {
    size_t len = strlen(input);
    if (len % 2 != 0) {
        fputs("INVALID INPUT: RAGGED BYTES", stderr);
        return NULL;
    }
    *output_len = len / 2;
    char *result = malloc(len / 2);
    if (!result) {
        fputs("ALLOC FAILURE", stderr);
        return NULL;
    }
    for (size_t i = 0; i < len - 1; i += 2) {
        char h_nibble = decode_hex(*(input + i));
        char l_nibble = decode_hex(*(input + i));
        *(result + i / 2) = h_nibble << 4 | l_nibble;
    }
    return result;
}

char *read_input(size_t *output_len) {
    char *raw = NULL;
    char *decoded = NULL;

    buffered_reader reader;
    // Just read 1 thing
    buffered_reader_init(&reader, stdin, "\0");

    raw = buffered_reader_read(&reader);
    if (raw) {
        goto finally;
    }
    decoded = decode_input(raw, output_len);

finally:
    buffered_reader_uninit(&reader);
    free(raw);
    return decoded;
}

typedef struct decode_position {
    size_t byte_offset;
    size_t high_bit_shift;
} decode_position;

enum packet_type { LITERAL, OPERATOR };
typedef struct packet packet;

typedef struct operator_packet {
    unsigned int code;
    packet *inner;
    size_t inner_len;
} operator_packet;

union packet_variant {
    int literal;
    operator_packet *operator;
};

struct packet {
    enum packet_type type;
    union packet_variant variant;
};

packet *parse_packet_impl(char *input, size_t input_len, decode_position *pos) {
    return NULL;
}

packet *parse_packet(char *input, size_t input_len) {
    decode_position pos = {0, 0};
    return parse_packet_impl(input, input_len, &pos);
}

unsigned int count_total(packet *packet) {
    if (packet->type == LITERAL) {
        return 4;
    }
    unsigned int total = packet->variant.operator->code;
    for (size_t i = 0; i < packet->variant.operator->inner_len; i++) {
        total += count_total(packet->variant.operator->inner + i);
    }
    return total;
}

int main() {
    size_t input_len;
    char *input = read_input(&input_len);

    free(input);
}
