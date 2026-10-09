#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "Stack/Stack.cpp"

//TODO safe!!!

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

const size_t       DEFAULT_STACK_SIZE = 8 ;
const unsigned int MAX_FILE_NAME      = 100;

enum

void processCMDArguments(const int argc, char* const * argv, char** const name_from_p);

void sub(stack_t* stack_p);
void add(stack_t* stack_p);
void out(stack_t* stack_p);
void end(stack_t* stack_p);

#endif
