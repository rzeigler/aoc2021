#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int read_into(input_buffer *in_buf, unsigned int *window, size_t offset) {
    char* line = input_buffer_read(in_buf);
    if (!line) {
        return 0;
    }
    *(window + offset) = atoi(line);
    free(line);
    return 1;
}

void slide(unsigned int *window) {
    *window = *(window + 1);
    *(window + 1) = *(window + 2);
}

unsigned int sum(unsigned int *window) {
    return *window + *(window + 1) + *(window + 2);
}

int main() {
    unsigned int window[3];
    unsigned int last_sum = 0;
    unsigned int current_sum = 0;
    unsigned int count = 0;

    input_buffer buffer;
    input_buffer_init(&buffer, stdin, "\n");

    read_into(&buffer, window, 0);
    read_into(&buffer, window, 1);
    read_into(&buffer, window, 2);

    last_sum = sum(window);

    slide(window);
    while (read_into(&buffer, window, 2)) {
        current_sum = sum(window);
        if (current_sum > last_sum) {
            count += 1;
        }
        
        last_sum = current_sum;
        slide(window);
    }

    input_buffer_uninit(&buffer);
    
    printf("%u\n", count);
}


