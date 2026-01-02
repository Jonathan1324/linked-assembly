#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

int32_t run_program(const char* cmd, const char* input, char** out_buf, char** err_buf);
void free_c_string(char* s);

#ifdef __cplusplus
}
#endif