#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include "Commands.h"
#include "Colors.h"

struct assembler_t{

    char*  asm_code;
    char*  byte_code;
    size_t byte_code_idx;
    size_t asm_code_idx;
    int    descr_out;
    size_t byte_code_len;
    size_t asm_code_len;
    size_t next_str_idx;
    size_t num_of_command;
};

void startAssembler(assembler_t* assembler_p, const int argc, char* const* argv);
void processCMDArguments(const int argc, char* const * argv, char** const name_from_p, char** const name_to_p);
void processTheCommand(assembler_t* assembler_p, const char* const command, const double argument);

#endif
