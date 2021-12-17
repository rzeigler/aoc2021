#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"

typedef struct position {
    int horizontal;
    int depth;
    int aim;
} position;

void step_position(position *pos, str *command, str *dist) {
    int dist_v = str_atoui(dist);
    if (!memcmp(command->str, "forward", command->str_len)) {
        pos->horizontal += dist_v;
        pos->depth += pos->aim * dist_v;
    } else if (!memcmp(command->str, "down", command->str_len)) {
        pos->aim += dist_v;
    } else if (!memcmp(command->str, "up", command->str_len)) {
        pos->aim -= dist_v;
    } else {
        puts("INVALID FORMAT");
    }
}

int main() {
    buffered_reader in_buf;
    tokenizer tokz;
    position pos = {0, 0, 0};

    buffered_reader_init(&in_buf, stdin, "\n");

    char *line = buffered_reader_read(&in_buf);
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

        line = buffered_reader_read(&in_buf);
    }

    buffered_reader_uninit(&in_buf);

    printf("%d\n", pos.horizontal * pos.depth);
}
