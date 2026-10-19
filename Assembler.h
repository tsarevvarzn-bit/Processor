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

const int    MAX_COMMAND_LEN = 100;
const int    MAX_MARK_NAME_LEN = 100;
const int    NUM_OF_MARKS = 10;
const size_t DEFAULT_MARK_VALUE = 0;



struct mark_t{

    char   name[MAX_MARK_NAME_LEN];
    size_t idx;
};

struct assembler_t{

    char*  asm_code;
    char*  byte_code;

    size_t asm_code_idx;
    size_t byte_code_idx;

    size_t asm_code_len;
    size_t byte_code_len;

    size_t next_str_idx;
    size_t num_of_command;

    mark_t marks[NUM_OF_MARKS];
    int    num_of_marks;

    int    num_of_completed_assemblies;

    int    descr_out;
};

void startAssembler(assembler_t* assembler_p, const int argc, char* const* argv);
void processCMDArguments(const int argc, char* const * argv, char** const name_from_p, char** const name_to_p);
void assembleTheCode(assembler_t* assembler_p);
void processNextCommand(assembler_t* assembler_p);
void checkAndResizeUpByteCode(assembler_t* assembler_p);

void processPush (assembler_t* assembler_p);
void processPop  (assembler_t* assembler_p);
void processJumps(assembler_t* assembler_p, const char* const command_name);
void processCall (assembler_t* assembler_p);
void processMark (assembler_t* assembler_p);

void decryptMark(assembler_t* assembler_p, size_t mark_idx);
bool findMark   (assembler_t* assembler_p, const char* const name, size_t* mark_idx_p);

void asmDump(assembler_t* assembler_p);

#endif
