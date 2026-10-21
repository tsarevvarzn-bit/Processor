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

typedef void (*function_t)(assembler_t* assembler_p);

struct command_t{

    char           name[MAX_COMMAND_LEN];
    commands_codes code                 ;
    function_t     function             ;
};

void processCMDArguments     (const int argc, char* const * argv, char** const name_from_p, char** const name_to_p);
void startAssembler          (assembler_t* assembler_p, const int argc, char* const* argv);
void assembleTheCode         (assembler_t* assembler_p);
void processNextCommand      (assembler_t* assembler_p);
void checkAndResizeUpByteCode(assembler_t* assembler_p);
bool isNotCommandStrCorrect  (assembler_t* assembler_p, const char* const command);
void asmDump                 (assembler_t* assembler_p);


void processPush(assembler_t* assembler_p);
void processPop (assembler_t* assembler_p);
void processJump(assembler_t* assembler_p);
void processJA  (assembler_t* assembler_p);
void processJAE (assembler_t* assembler_p);
void processJB  (assembler_t* assembler_p);
void processJBE (assembler_t* assembler_p);
void processJE  (assembler_t* assembler_p);
void processJNE (assembler_t* assembler_p);
void processCall(assembler_t* assembler_p);
void processMark(assembler_t* assembler_p);

void decryptMark    (assembler_t* assembler_p, size_t mark_idx);
bool findMark       (assembler_t* assembler_p, const char* const name, size_t* mark_idx_p);
void processRegister(assembler_t* assembler_p, const char* const name_of_reg);


command_t commands[]{

    {.name = "PUSH", .code = ZERO, .function = processPush},
    {.name = "POP",  .code = ZERO, .function = processPop },

    {.name = "JUMP", .code = ZERO, .function = processJump},
    {.name = "JA"  , .code = ZERO, .function = processJA  },
    {.name = "JAE" , .code = ZERO, .function = processJAE },
    {.name = "JB"  , .code = ZERO, .function = processJB  },
    {.name = "JBE" , .code = ZERO, .function = processJBE },
    {.name = "JE"  , .code = ZERO, .function = processJE  },
    {.name = "JNE" , .code = ZERO, .function = processJNE },

    {.name = "CALL", .code = ZERO, .function = processCall},
    {.name = "RET" , .code = RET , .function = NULL       },
    {.name = "IN"  , .code = IN  , .function = NULL       },
    {.name = "OUT" , .code = OUT , .function = NULL       },
    {.name = "HLT" , .code = HLT , .function = NULL       },

    {.name = "ADD" , .code = ADD , .function = NULL       },
    {.name = "SUB" , .code = SUB , .function = NULL       },
    {.name = "MUL" , .code = MUL , .function = NULL       },
    {.name = "DIV" , .code = DIV , .function = NULL       },
    {.name = "SQRT", .code = SQRT, .function = NULL       },

    //{.name = ":"   , .code = ZERO, .function = processMark},
};

#endif
