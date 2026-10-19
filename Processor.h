#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "Stack/Stack.cpp"
#include "Commands.h"
//TODO safe!!!

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

const size_t       DEFAULT_STACK_SIZE = 8 ;
const unsigned int MAX_FILE_NAME      = 100;
const double       EPSILON            = 0.0001;

struct processor_t{

    stack_t*       computing_stack;
    stack_t*       returns_stack;
    unsigned char* byte_code;
    size_t         i_p;
    size_t         len_of_byte_code;

    elem_type         r_a;
    elem_type         r_b;
    elem_type         r_c;
    elem_type         r_d;
};

void       startProcessor     (processor_t* processor_p, const int argc, char* const* argv);
void       processCMDArguments(const int argc, char* const * argv, char** const name_from_p);

void       procDump           (processor_t* processor_p);

void       safeStackPop       (stack_t*  stack_p, elem_type* elem_p);
elem_type* getRegisterPointer (processor_t* processor_p, unsigned char register_code);

void push (processor_t* processor_p);
void popR (processor_t* processor_p);
void jump (processor_t* processor_p);
void call (processor_t* processor_p);
void ret  (processor_t* processor_p);
void out  (processor_t* processor_p);
void hlt  (processor_t* processor_p);

void add  (processor_t* processor_p);
void sub  (processor_t* processor_p);
void mul  (processor_t* processor_p);
void div  (processor_t* processor_p);
void sqrt (processor_t* processor_p);

bool isZero(elem_type number);

typedef void (*function_t)(processor_t* processor_p);

const function_t COMMANDS_FUNCTIONS[] = {

    push,
    popR,
    jump,
    call,
    ret,
    out,
    hlt,

    add,
    sub,
    mul,
    div,
    sqrt
};

#endif
//
// #ifdef FLAG1
// ....
// #endif
//TODO -D_FLAG1
