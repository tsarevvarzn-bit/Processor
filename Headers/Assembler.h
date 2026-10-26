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

const int    MAX_COMMAND_LEN    = 100;
const int    MAX_MARK_NAME_LEN  = 100;
const int    NUM_OF_MARKS       =  10;
const size_t DEFAULT_MARK_VALUE =   0;

enum return_codes{

    CONTINUE = 0,
    END      = 1,
};

struct mark_t{

    char   name[MAX_MARK_NAME_LEN];
    size_t idx;
};

struct assembler_t{

    char*  asm_code ;
    char*  byte_code;

    size_t asm_code_idx ;
    size_t byte_code_idx;

    size_t asm_code_len ;
    size_t byte_code_len;

    size_t next_str_idx  ;
    size_t num_of_command;

    mark_t marks[NUM_OF_MARKS];
    int    num_of_marks       ;

    int    num_of_completed_assemblies;

    int    descr_out;
};

struct pushPopConfig_t{

    double        arg_number_imm ;
    unsigned char arg_reg_code   ;

    unsigned char command        ;
};

typedef return_codes (*function_t)(assembler_t* assembler_p);

struct command_t{

    char           name[MAX_COMMAND_LEN];
    commands_codes code                 ;
    function_t     function             ;
};

return_codes processCMDArguments     (const int argc, char* const * argv, char** const name_from_p, char** const name_to_p);
return_codes startAssembler          (assembler_t* assembler_p, const int argc, char* const* argv);
return_codes assembleTheCode         (assembler_t* assembler_p);
return_codes processNextCommand      (assembler_t* assembler_p);
return_codes checkAndResizeUpByteCode(assembler_t* assembler_p);
bool isNotCommandStrCorrect          (assembler_t* assembler_p, const char* const command);
void asmDump                         (assembler_t* assembler_p);


return_codes processPush (assembler_t* assembler_p);
return_codes processPop  (assembler_t* assembler_p);
return_codes processJump (assembler_t* assembler_p);
return_codes processJA   (assembler_t* assembler_p);
return_codes processJAE  (assembler_t* assembler_p);
return_codes processJB   (assembler_t* assembler_p);
return_codes processJBE  (assembler_t* assembler_p);
return_codes processJE   (assembler_t* assembler_p);
return_codes processJNE  (assembler_t* assembler_p);
return_codes processCall (assembler_t* assembler_p);
return_codes processDelay(assembler_t* assembler_p);
return_codes processMark (assembler_t* assembler_p);

return_codes decryptMark    (assembler_t* assembler_p, size_t mark_idx);
bool findMark               (assembler_t* assembler_p, const char* const name, size_t* mark_idx_p);
return_codes processRegister(assembler_t* assembler_p, const char* const name_of_reg, unsigned char* const reg_code_p);

return_codes parsePushPopArgument        (pushPopConfig_t* pushConfig_p,    assembler_t* assembler_p, const int len_of_command);
return_codes parsePushPopWithOneArgument (pushPopConfig_t* pushPopConfig_p, assembler_t* assembler_p, const char* const first_arg_str, const char* const second_arg_str);
return_codes parsePushPopWithTwoArguments(pushPopConfig_t* pushPopConfig_p, assembler_t* assembler_p, const char* const first_arg_str, const char* const second_arg_str);
//TODO градация серого в рейлибе на примере кружочка, 255 оттенков

const command_t commands[]{

    {.name = "PUSH" , .code = ZERO, .function = processPush },
    {.name = "POP"  , .code = ZERO, .function = processPop  },

    {.name = "JUMP" , .code = ZERO, .function = processJump },
    {.name = "JA"   , .code = ZERO, .function = processJA   },
    {.name = "JAE"  , .code = ZERO, .function = processJAE  },
    {.name = "JB"   , .code = ZERO, .function = processJB   },
    {.name = "JBE"  , .code = ZERO, .function = processJBE  },
    {.name = "JE"   , .code = ZERO, .function = processJE   },
    {.name = "JNE"  , .code = ZERO, .function = processJNE  },

    {.name = "CALL" , .code = ZERO, .function = processCall },
    {.name = "RET"  , .code = RET , .function = NULL        },
    {.name = "IN"   , .code = IN  , .function = NULL        },
    {.name = "OUT"  , .code = OUT , .function = NULL        },
    {.name = "DRAW" , .code = DRAW, .function = NULL        },
    {.name = "DELAY", .code = ZERO, .function = processDelay},
    {.name = "HLT"  , .code = HLT , .function = NULL        },

    {.name = "ADD"  , .code = ADD , .function = NULL        },
    {.name = "SUB"  , .code = SUB , .function = NULL        },
    {.name = "MUL"  , .code = MUL , .function = NULL        },
    {.name = "DIV"  , .code = DIV , .function = NULL        },
    {.name = "SQRT" , .code = SQRT, .function = NULL        },
};

#endif
