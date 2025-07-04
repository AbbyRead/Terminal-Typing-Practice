#ifndef PROMPT_H
#define PROMPT_H

#include "buffers.h"

#define MAX_LINE_LEN 2048

line_array_t *prompt_user(const line_array_t *prompt, size_t start_line);

#endif
