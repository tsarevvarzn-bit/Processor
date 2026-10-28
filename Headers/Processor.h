#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "..\Raylib\raylib.h"

#undef RED
#undef GREEN
#undef YELLOW
#undef BLUE
#undef VIOLET
#undef CYAN

#include "..\Stack\Stack.cpp"

#include "Commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

const size_t DEFAULT_STACK_SIZE = 8     ;
const int    MAX_FILE_NAME      = 100   ;
const double EPSILON            = 0.0001;
const int    RAM_SIZE           = 50    ;
const int    CANVAS_WIDTH       = 250   ;
const int    CANVAS_HEIGHT      = 250   ;
const int    PIXEL_SIZE         = 4     ;

const size_t VRAM_SIZE          = CANVAS_WIDTH * CANVAS_HEIGHT;


struct processor_t{

    stack_t*       computing_stack ;
    stack_t*       returns_stack   ;
    unsigned char* byte_code       ;
    size_t         i_p             ;
    size_t         len_of_byte_code;

    elem_type      r_a;
    elem_type      r_b;
    elem_type      r_c;
    elem_type      r_d;

    elem_type      RAM[VRAM_SIZE + RAM_SIZE];
};

enum return_codes{

    CONTINUE = 0,
    END      = 1,
};

void         startProcessor     (processor_t* processor_p, const int argc, char* const* argv);
void         procDump           (processor_t* processor_p);
elem_type*   getRegisterPointer (processor_t* processor_p, unsigned char register_code);
void         endOfWorking       (processor_t* processor_p);

void         processCMDArguments(const int argc,    char* const * argv, char** const name_from_p);
return_codes safeStackPop       (stack_t*  stack_p, elem_type*    elem_p);
bool         isZero             (elem_type number);

bool         isJumping          (const unsigned char argument,
                                 const elem_type     left_elem,
                                 const elem_type     right_elem,
                                 return_codes* const return_code_p);

return_codes pop  (processor_t* processor_p);
return_codes push (processor_t* processor_p);
return_codes jump (processor_t* processor_p);
return_codes call (processor_t* processor_p);
return_codes ret  (processor_t* processor_p);
return_codes in   (processor_t* processor_p);
return_codes out  (processor_t* processor_p);
return_codes draw (processor_t* processor_p);
return_codes delay(processor_t* processor_p);
return_codes hlt  (processor_t* processor_p);

return_codes add  (processor_t* processor_p);
return_codes sub  (processor_t* processor_p);
return_codes mul  (processor_t* processor_p);
return_codes div  (processor_t* processor_p);
return_codes sqrt (processor_t* processor_p);
return_codes rnd  (processor_t* processor_p);


return_codes popRAM(processor_t* processor_p, elem_type elem, unsigned char argument);
return_codes popReg(processor_t* processor_p, elem_type elem);

typedef return_codes (*function_t)(processor_t* processor_p);

const function_t COMMANDS_FUNCTIONS[] = {

    push ,
    pop  ,
    jump ,
    call ,
    ret  ,
    in   ,
    out  ,
    draw ,
    delay,
    hlt  ,

    add  ,
    sub  ,
    mul  ,
    div  ,
    sqrt ,
    rnd
};

#ifdef COMMANDS_DUMP

    #define ON_COMMANDS_DUMP(...) __VA_ARGS__

#else

    #define ON_COMMANDS_DUMP(...)

#endif

#ifdef PROC_STRUCT_DUMP

    #define ON_PROC_STRUCT_DUMP(...) __VA_ARGS__

#else

    #define ON_PROC_STRUCT_DUMP(...)

#endif

#ifdef CONSOLE_DRAW

    #define ON_CONSOLE_DRAW(...) __VA_ARGS__

#else

    #define ON_CONSOLE_DRAW(...)

#endif



#endif //endif #ifndef PROCESSOR_H
