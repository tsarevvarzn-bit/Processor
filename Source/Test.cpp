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

    double x = 500;

    size_t y = (size_t) x;

    printf("%llu", y);
}
