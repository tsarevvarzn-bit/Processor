#include "Processor.h"



int main(const int argc, char* const* argv){

    processor_t processor = {};
    startProcessor(&processor, argc, argv);
    putchar('\n');

    unsigned char command = 255;

    while(processor.i_p < processor.len_of_byte_code){

        command = processor.byte_code[processor.i_p++] & 0b00011111;

        //printf("Start doing command: <%X>\n", command);

        if(command < PUSH || command > SQRT){

            printf(RED "INCORRECT COMMAND IN %llu BYTE\n" DEFAULT, processor.i_p);
            hlt(&processor);
        }

        (COMMANDS_FUNCTIONS[command - 1])(&processor);

        //printf("Done command: <%X>\n", command);
        //procDump(&processor);
        //Более гибкая система, пауза после каждой: d - dump, иначе - не дамп
    }
}

void startProcessor(processor_t* processor_p, const int argc, char* const* argv){

    assert(processor_p);

    stack_t* computing_stack_p = NULL;
    stack_t* returns_stack_p   = NULL;

    createAndInitStack(&computing_stack_p, DEFAULT_STACK_SIZE);
    createAndInitStack(&returns_stack_p, DEFAULT_STACK_SIZE);

    processor_p->computing_stack = computing_stack_p;
    processor_p->returns_stack   = returns_stack_p;

    char* in_file_name_p = NULL;

    processCMDArguments(argc, argv, &in_file_name_p);

    int descr = open(in_file_name_p, _O_BINARY);

    struct stat file_stats = {};
    fstat(descr, &file_stats);

    processor_p->byte_code = (unsigned char*) calloc(file_stats.st_size, sizeof(unsigned char));
    read(descr, processor_p->byte_code, file_stats.st_size);
    processor_p->len_of_byte_code = file_stats.st_size;
    processor_p->i_p = 0;

    processor_p->r_a = POISON;
    processor_p->r_b = POISON;
    processor_p->r_c = POISON;
    processor_p->r_d = POISON;

    printf("Len of byte code: %llu", processor_p->len_of_byte_code);

    close(descr);
}


void push(processor_t* processor_p){

    assert(processor_p);

    unsigned char argument = processor_p->byte_code[processor_p->i_p - 1] & 0b11100000;
    elem_type elem = POISON;

    switch(argument){

        case PUSH_ARG_IMMED_CONST:

            elem = *((elem_type*) (processor_p->byte_code + processor_p->i_p));
            //printf("Imm const\n");
            processor_p->i_p += sizeof(elem_type);
            break;

        case PUSH_ARG_REG:

            {
            unsigned char register_code = *((unsigned char*) (processor_p->byte_code + processor_p->i_p++));
            elem_type* register_p = NULL;

            if((register_p = getRegisterPointer(processor_p, register_code)) != NULL){

                elem = *register_p;

            }else{

                printf(RED "ERROR: incorrect argument of PUSHr, code of register: <%hhu>" DEFAULT, register_code);
                hlt(processor_p);
            }

            if(IS_POISON(elem)){

                printf(RED "ERROR: NAN value of register, code of register: <%hhu>\n" DEFAULT, register_code);
                hlt(processor_p);
            }

            //printf("Reg const\n");

            break;
            }

        default:

            printf(RED "ERROR: incorrect argument of PUSH in left 3 bits, push argument: <%X>" DEFAULT, argument);
            hlt(processor_p);
    }

    stackPush(processor_p->computing_stack, elem);
}

void popR(processor_t* processor_p){

    assert(processor_p);

    unsigned char register_code = *((unsigned char*) (processor_p->byte_code + processor_p->i_p++));
    elem_type* register_p = NULL;

    if((register_p = getRegisterPointer(processor_p, register_code)) != NULL){

        safeStackPop(processor_p->computing_stack, register_p);
        return;
    }

    printf(RED "ERROR: incorrect argument of POPr, code of register: <%hhu>" DEFAULT, register_code);
    hlt(processor_p);
}


void jump(processor_t* processor_p){

    assert(processor_p);

    unsigned char argument = processor_p->byte_code[processor_p->i_p - 1] & 0b11100000;

    size_t jump_index = *((size_t*) (processor_p->byte_code + processor_p->i_p));
    processor_p->i_p += sizeof(size_t);

    if(argument != JUMP_ARG_ABS){

        elem_type left_elem  = NAN;
        elem_type right_elem = NAN;

        safeStackPop(processor_p->computing_stack, &right_elem);
        safeStackPop(processor_p->computing_stack, &left_elem);

        switch(argument){//Функция разбора аргумента

            case JUMP_ARG_A:

                if(! (left_elem > right_elem))
                    return;
                break;

            case JUMP_ARG_AE:

                if(! (left_elem >= right_elem))
                    return;
                break;

            case JUMP_ARG_B:

                if(! (left_elem < right_elem))
                    return;
                break;

            case JUMP_ARG_BE:

                if(! (left_elem <= right_elem))
                    return;
                break;

            case JUMP_ARG_E:

                if(! isZero(left_elem - right_elem))
                    return;
                break;

            case JUMP_ARG_NE:

                if(!! isZero(left_elem - right_elem))
                    return;
                break;

            default:

                printf(RED "ERROR: incorrect argument of JUMP in left 3 bits, jump argument: <%X>" DEFAULT, argument);
                hlt(processor_p);

        }
    }

    //printf("Doing jump\n");
    processor_p->i_p = jump_index;
}

void call(processor_t* processor_p){

    assert(processor_p);

    //printf("1\n");

    size_t ret_idx = processor_p->i_p + sizeof(size_t);

    elem_type ret_i_p_double = *((elem_type*)(&ret_idx));

    //printf("2\n");

    stackPush(processor_p->returns_stack, ret_i_p_double);
    //printf("LABUBA in: %lg, %llu\n", ret_i_p_double, *((size_t*) (&ret_i_p_double)));

    //printf("3, proc ip: %llu\n", processor_p->i_p);

    processor_p->i_p = *((size_t*) (processor_p->byte_code + processor_p->i_p));

    //printf("4\n");
}

void ret(processor_t* processor_p){

    assert(processor_p);

    elem_type ret_i_p_double = POISON;

    safeStackPop(processor_p->returns_stack, &ret_i_p_double);

    //printf("LABUBA out: %lg\n", ret_i_p_double);

    //printf("Old stack_ip: %llu\n", processor_p->i_p);

    processor_p->i_p = *((size_t*) (&ret_i_p_double));

    //printf("New stack_ip: %llu\n", processor_p->i_p);
}

void out(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;

    safeStackPop(processor_p->computing_stack, &a);

    printf(GREEN BOLD "OUT: %lg\n" DEFAULT, a);
}

void hlt(processor_t* processor_p){

    assert(processor_p);

    stackDelete(processor_p->computing_stack);

    free(processor_p->byte_code);

    printf("HLT\n");

    exit(errno);
}

void sub(processor_t* processor_p){//Отдельная функция для всех бинарных функций

    assert(processor_p);

    elem_type a = NAN;
    elem_type b = NAN;

    safeStackPop(processor_p->computing_stack, &b);
    safeStackPop(processor_p->computing_stack, &a);

    stackPush(processor_p->computing_stack, a - b);
}

void add(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;
    elem_type b = NAN;

    safeStackPop(processor_p->computing_stack, &b);
    safeStackPop(processor_p->computing_stack, &a);

    stackPush(processor_p->computing_stack, a + b);
}

void mul(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;
    elem_type b = NAN;

    safeStackPop(processor_p->computing_stack, &b);
    safeStackPop(processor_p->computing_stack, &a);

    stackPush(processor_p->computing_stack, a * b);
}

void div(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;
    elem_type b = NAN;

    safeStackPop(processor_p->computing_stack, &b);
    safeStackPop(processor_p->computing_stack, &a);

    stackPush(processor_p->computing_stack, a / b);
}

void sqrt(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;

    safeStackPop(processor_p->computing_stack, &a);

    stackPush(processor_p->computing_stack, pow(a, 0.5));
}

void procDump(processor_t* processor_p){

    assert(processor_p);

    //Smart dump, чекает изменения проца

    printf(VIOLET "\n---------------------------------------PROC_DUMP---------------------------------------\n\n" DEFAULT);

    printf(YELLOW "Registers:\n\n" DEFAULT);
    printf("AX: " BLUE "%lg\n"   DEFAULT, processor_p->r_a);
    printf("BX: " BLUE "%lg\n"   DEFAULT, processor_p->r_b);
    printf("CX: " BLUE "%lg\n"   DEFAULT, processor_p->r_c);
    printf("DX: " BLUE "%lg\n\n" DEFAULT, processor_p->r_d);


    printf(YELLOW "Calculation stack: \n" DEFAULT);
    printStack(processor_p->computing_stack);

    printf(YELLOW "Returns stack: \n" DEFAULT);
    printStack(processor_p->returns_stack);


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
    printf(DEFAULT "\nWe are in idx = %llu", processor_p->i_p);

    printf("\n\n" DEFAULT);
    printf(VIOLET "\n---------------------------------------END_OF_DUMP--------------------------------\n\n" DEFAULT);
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

double* getRegisterPointer(processor_t* processor_p, unsigned char register_code){

    assert(processor_p);

    switch(register_code){

        case AX:

            return &processor_p->r_a;

        case BX:

            return &processor_p->r_b;

        case CX:

            return &processor_p->r_c;

        case DX:

            return &processor_p->r_d;

        default:

            return NULL;
    }
}

void safeStackPop(stack_t* stack_p, elem_type* elem_p){

    assert(stack_p);
    assert(elem_p);

    stackPop(stack_p, elem_p);

    if(IS_POISON(*elem_p)){

        exit(errno);//!!!!!!!!!!!!
    }
}

bool isZero(elem_type number){

    return (abs(number) < EPSILON) ? true : false;
}
