#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

typedef struct position {
    int horizontal;
    int depth;
} position;

void step_position(position *pos, str *command, str *dist) {
    int dist_v = lazy_str_atoi(dist);
    if (!memcmp(command->str, "forward", command->str_len)) {
        pos->horizontal += dist_v;
    } else if (!memcmp(command->str, "down", command->str_len)) {
        pos->depth += dist_v;
    } else if (!memcmp(command->str, "up", command->str_len)) {
        pos->depth -= dist_v;
    } else {
        puts("INVALID FORMAT");
    }
}

int main() {
    input_buffer in_buf;
    tokenizer tokz;
    position pos = {0, 0};

    input_buffer_init(&in_buf, stdin, "\n");

    char *line = input_buffer_read(&in_buf);
    while (line) {
        
        str command;
        str distance;

        tokenizer_init(&tokz, line, " ");

        command.str = tokenizer_next(&tokz, &command.str_len);
        distance.str = tokenizer_next(&tokz, &distance.str_len);

        if (!command.str || !distance.str) {
            fputs("INVALID INPUT FORMAT", stderr);
        } else {
            step_position(&pos, &command, &distance);
        }

        tokenizer_uninit(&tokz);

        line = input_buffer_read(&in_buf);
    }

    input_buffer_uninit(&in_buf);

    printf("%d\n", pos.horizontal * pos.depth);
}
