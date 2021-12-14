#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    input_buffer *buffer = input_buffer_create(stdin, "\n");
    char *line = input_buffer_read(buffer);
    while (line) {
        printf("%s\n", line);
        free(line);
        line = input_buffer_read(buffer);
    }
    free(line);
    input_buffer_release(buffer);
}
