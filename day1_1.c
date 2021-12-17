#include <stdio.h>
#include <stdlib.h>

#include "io.h"

int main() {
    unsigned int last_num = 0;
    unsigned int current_num = 0;
    unsigned int increases = 0;

    buffered_reader buffer;
    buffered_reader_init(&buffer, stdin, "\n");

    char *line = buffered_reader_read(&buffer);
    if (!line) {
        printf("INPUT FORMAT IS BROKEN");
    }
    last_num = atoi(line);
    free(line);

    line = buffered_reader_read(&buffer);
    while (line) {
        current_num = atoi(line);
        free(line);
        if (current_num > last_num) {
            increases += 1;
        }
        last_num = current_num;

        line = buffered_reader_read(&buffer);
    }

    buffered_reader_uninit(&buffer);

    printf("%u\n", increases);
}
