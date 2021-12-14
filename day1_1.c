#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    unsigned int last_num = 0;
    unsigned int current_num = 0;
    unsigned int increases = 0;

    input_buffer *buffer = input_buffer_create(stdin, "\n");

    char *line = input_buffer_read(buffer);
    if (!line) {
        printf("INPUT FORMAT IS BROKEN");
    }
    last_num = atoi(line);
    free(line);

    line = input_buffer_read(buffer);
    while (line) {
        current_num = atoi(line);
        free(line);
        if (current_num > last_num) {
            increases += 1;
        }
        last_num = current_num;

        line = input_buffer_read(buffer);
    }

    input_buffer_release(buffer);

    printf("%u\n", increases);
}
