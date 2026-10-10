#include "Processor.h"



int main(const int argc, char* const* argv){

    stack_t* stack_p = NULL;
    createAndInitStack(&stack_p, DEFAULT_STACK_SIZE);

    char* in_file_name_p = NULL;

    processCMDArguments(argc, argv, &in_file_name_p);

    int descr = open(in_file_name_p, _O_BINARY);

    struct stat file_stats = {};
    fstat(descr, &file_stats);

    processor_t processor = {};
    processor.stack_p = stack_p;
    processor.byte_code = (unsigned char*) calloc(file_stats.st_size, sizeof(unsigned char));
    read(descr, processor.byte_code, file_stats.st_size);
    processor.IP = 0;

    unsigned char command = 255;


    while(processor.IP < file_stats.st_size){

        command = processor.byte_code[processor.IP++];

        printf("Detected command: <%hhu>\n", command - '0');

        switch (command){

            case PUSH:

                push(&processor);
                break;

            case ADD:

                add(&processor);
                break;

            case SUB:

                sub(&processor);
                break;

            case OUT:

                out(&processor);
                break;

            case HLT:

                hlt(&processor);
                break;

            default:

                printf(RED "INCORRECT COMMAND IN %ld BYTE" DEFAULT, processor.IP);
                hlt(&processor);
                break;
        }
    }

}

void processCMDArguments(const int argc, char* const * argv, char** const name_from_p){

    assert(argv);
    assert(name_from_p);

    int getopt_out = 0;

    while ((getopt_out = getopt(argc, argv, "f:")) != -1){

        switch (getopt_out){

            case 'f':
                *name_from_p = optarg;
                break;

            default:
                break;
        }
    }

    if(*name_from_p == NULL){

        printf(RED "ERROR: please, enter correct CMD arguments: -f name_of_input_file.txt\n" DEFAULT);
        exit(EXIT_FAILURE);
    }

}

void push(processor_t* processor_p){

    assert(processor_p);

    elem_type elem = *((elem_type*) (processor_p->byte_code + processor_p->IP));

    stackPush(processor_p->stack_p, elem);

    processor_p->IP += sizeof(elem_type);
}

void sub(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;
    elem_type b = NAN;

    stackPop(processor_p->stack_p, &b);
    stackPop(processor_p->stack_p, &a);

    stackPush(processor_p->stack_p, a - b);
}

void add(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;
    elem_type b = NAN;

    stackPop(processor_p->stack_p, &b);
    stackPop(processor_p->stack_p, &a);

    stackPush(processor_p->stack_p, a + b);
}

void out(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;

    stackPop(processor_p->stack_p, &a);

    printf("OUT: %lg\n", a);
}

void hlt(processor_t* processor_p){

    assert(processor_p);

    stackDelete(processor_p->stack_p);

    free(processor_p->byte_code);

    printf("HLT\n");

    exit(errno);
}
