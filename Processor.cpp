#include "Processor.h"



int main(const int argc, char* const* argv){

    processor_t processor = {};
    startProcessor(&processor, argc, argv);
    putchar('\n');

    unsigned char command = 255;

    while(processor.i_p < processor.len_of_byte_code){

        command = processor.byte_code[processor.i_p++];

        printf("Detected command: <%hhu>\n", command - '0');

        switch (command){

            case PUSH:

                push(&processor);
                break;

            case PUSHr:

                pushR(&processor);
                break;

            case POPr:

                popR(&processor);
                break;

            case OUT:

                out(&processor);
                break;

            case HLT:

                hlt(&processor);
                break;

            case ADD:

                add(&processor);
                break;

            case SUB:

                sub(&processor);
                break;


            case MUL:

                mul(&processor);
                break;

            case DIV:

                div(&processor);
                break;

            case SQRT:

                sqrt(&processor);
                break;

            default:

                printf(RED "INCORRECT COMMAND IN %ld BYTE\n" DEFAULT, processor.i_p);
                hlt(&processor);
                break;
        }

        procDump(&processor);
    }

}

void startProcessor(processor_t* processor_p, const int argc, char* const* argv){

    assert(processor_p);

    stack_t* stack_p = NULL;
    createAndInitStack(&stack_p, DEFAULT_STACK_SIZE);

    char* in_file_name_p = NULL;

    processCMDArguments(argc, argv, &in_file_name_p);

    int descr = open(in_file_name_p, _O_BINARY);

    struct stat file_stats = {};
    fstat(descr, &file_stats);

    processor_p->stack_p = stack_p;
    processor_p->byte_code = (unsigned char*) calloc(file_stats.st_size, sizeof(unsigned char));
    read(descr, processor_p->byte_code, file_stats.st_size);
    processor_p->len_of_byte_code = file_stats.st_size;
    processor_p->i_p = 0;

    processor_p->r_a = POISON;
    processor_p->r_b = POISON;
    processor_p->r_c = POISON;
    processor_p->r_d = POISON;

    close(descr);
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

    elem_type elem = *((elem_type*) (processor_p->byte_code + processor_p->i_p));

    stackPush(processor_p->stack_p, elem);

    processor_p->i_p += sizeof(elem_type);
}

void pushR(processor_t* processor_p){

    assert(processor_p);

    unsigned char register_number = *((unsigned char*) (processor_p->byte_code + processor_p->i_p++));

    switch(register_number){

        case AX:

            stackPush(processor_p->stack_p, processor_p->r_a);
            break;

        case BX:

            stackPush(processor_p->stack_p, processor_p->r_b);
            break;

        case CX:

            stackPush(processor_p->stack_p, processor_p->r_c);
            break;

        case DX:

            stackPush(processor_p->stack_p, processor_p->r_d);
            break;

        default:

            printf(RED "ERROR: incorrect argument of PUSHr, code of register: <%hhu>" DEFAULT, register_number);

    }

}

void popR(processor_t* processor_p){

    assert(processor_p);

    unsigned char register_number = *((unsigned char*) (processor_p->byte_code + processor_p->i_p++));

    switch(register_number){

        case AX:

            stackPop(processor_p->stack_p, &processor_p->r_a);
            break;

        case BX:

            stackPop(processor_p->stack_p, &processor_p->r_b);
            break;

        case CX:

            stackPop(processor_p->stack_p, &processor_p->r_c);
            break;

        case DX:

            stackPop(processor_p->stack_p, &processor_p->r_d);
            break;

        default:

            printf(RED "ERROR: incorrect argument of POPr, code of register: <%hhu>" DEFAULT, register_number);

    }

}

void out(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;

    safeStackPop(processor_p, &a);

    printf(RED "OUT: %lg\n" DEFAULT, a);
}

void hlt(processor_t* processor_p){

    assert(processor_p);

    stackDelete(processor_p->stack_p);

    free(processor_p->byte_code);

    printf("HLT\n");

    exit(errno);
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

    safeStackPop(processor_p, &b);
    safeStackPop(processor_p, &a);

    stackPush(processor_p->stack_p, a + b);
}

void mul(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;
    elem_type b = NAN;

    safeStackPop(processor_p, &b);
    safeStackPop(processor_p, &a);

    stackPush(processor_p->stack_p, a * b);
}

void div(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;
    elem_type b = NAN;

    safeStackPop(processor_p, &b);
    safeStackPop(processor_p, &a);

    stackPush(processor_p->stack_p, a / b);
}

void sqrt(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;

    safeStackPop(processor_p, &a);

    stackPush(processor_p->stack_p, pow(a, 0.5));
}

void procDump(processor_t* processor_p){

    assert(processor_p);

    printf(VIOLET "\n---------------------------------------PROC_DUMP---------------------------------------\n\n" DEFAULT);

    printf(YELLOW "Registers:\n\n" DEFAULT);
    printf("AX: " BLUE "%lg\n"   DEFAULT, processor_p->r_a);
    printf("BX: " BLUE "%lg\n"   DEFAULT, processor_p->r_b);
    printf("CX: " BLUE "%lg\n"   DEFAULT, processor_p->r_c);
    printf("DX: " BLUE "%lg\n\n" DEFAULT, processor_p->r_d);


    printf(YELLOW "Calculation stack: \n" DEFAULT);
    printStack(processor_p->stack_p);


    printf(YELLOW "\n\nCode:\n\n" DEFAULT);

    for(long unsigned int i = 0; i < processor_p->len_of_byte_code; i++){

        if(i < processor_p->i_p){

            printf(GREEN);

        }else if(i == processor_p->i_p){

            printf(YELLOW);

        }else{

            printf(BLUE);
        }

        putchar(processor_p->byte_code[i]);
    }

    printf("\n\n" DEFAULT);
    printf(VIOLET "\n---------------------------------------END_OF_DUMP--------------------------------\n\n" DEFAULT);
}

void safeStackPop(processor_t* processor_p, elem_type* elem_p){

    assert(processor_p);
    assert(elem_p);

    if(stackPop(processor_p->stack_p, elem_p) != correct){

        hlt(processor_p);
    }
}
