#include <stdio.h>
typedef struct input_buffer input_buffer;

input_buffer *input_buffer_create(FILE *file, char *buffer_by);
void input_buffer_release(input_buffer *in_buf);

char *input_buffer_read(input_buffer *buf);
