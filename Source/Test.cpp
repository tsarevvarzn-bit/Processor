//#include "Processor.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

int main(){

    unsigned char x = 0xB;
    x = x | 0b10010000;

    printf("%X", x);
}
