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

struct processor_t{

    stack_t*          stack_p;
    unsigned char*    byte_code;
    long unsigned int i_p;
    long unsigned int len_of_byte_code;

    elem_type         r_a;
    elem_type         r_b;
    elem_type         r_c;
    elem_type         r_d;
};

void startProcessor(processor_t* processor_p, const int argc, char* const* argv);

void procDump(processor_t* processor_p);

void processCMDArguments(const int argc, char* const * argv, char** const name_from_p);

void safeStackPop(processor_t* processor_p, elem_type* elem_p);

void push (processor_t* processor_p);
void pushR(processor_t* processor_p);
void popR (processor_t* processor_p);
void out  (processor_t* processor_p);
void hlt  (processor_t* processor_p);

void add  (processor_t* processor_p);
void sub  (processor_t* processor_p);
void mul  (processor_t* processor_p);
void div  (processor_t* processor_p);
void sqrt (processor_t* processor_p);



#endif
