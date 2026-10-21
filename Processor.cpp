#include "Processor.h"



int main(const int argc, char* const* argv){

    processor_t processor = {};
    startProcessor(&processor, argc, argv);

    unsigned char command = 0;

    while(processor.i_p < processor.len_of_byte_code){

        command = processor.byte_code[processor.i_p++] & 0b0001'1111;

        ON_COMMANDS_DUMP(printf("Start doing command: <%X>\n", command);)

        return_codes return_code = (COMMANDS_FUNCTIONS[command - PUSH])(&processor);

        ON_COMMANDS_DUMP(printf("Done command: <%X>\n", command);)
        ON_PROC_STRUCT_DUMP(procDump(&processor);)

        if(return_code == END)
            break;
    }

    endOfWorking(&processor);
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

    ON_COMMANDS_DUMP(printf("Len of byte code: %llu", processor_p->len_of_byte_code);)

    close(descr);
    putchar('\n');
}


return_codes push(processor_t* processor_p){

    assert(processor_p);

    unsigned char argument = processor_p->byte_code[processor_p->i_p - 1] & 0b1110'0000;
    elem_type elem = POISON;

    switch(argument){

        case PUSH_ARG_IMMED_CONST:

            elem = *((elem_type*) (processor_p->byte_code + processor_p->i_p));
            ON_COMMANDS_DUMP(printf("Imm const\n");)
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
                return END;
            }

            if(IS_POISON(elem)){

                printf(RED "ERROR: NAN value of register, code of register: <%hhu>\n" DEFAULT, register_code);
                return END;
            }

            ON_COMMANDS_DUMP(printf("Reg const\n");)

            break;
            }

        default:

            printf(RED "ERROR: incorrect argument of PUSH in left 3 bits, push argument: <%X>" DEFAULT, argument);
            return END;
    }

    stackPush(processor_p->computing_stack, elem);

    return CONTINUE;
}

return_codes popR(processor_t* processor_p){

    assert(processor_p);

    unsigned char register_code = *((unsigned char*) (processor_p->byte_code + processor_p->i_p++));
    elem_type* register_p = NULL;

    if((register_p = getRegisterPointer(processor_p, register_code)) != NULL){

        stackPop(processor_p->computing_stack, register_p);

        if(IS_POISON(*register_p)){

            return END;
        }

        return CONTINUE;
    }

    printf(RED "ERROR: incorrect argument of POPr, code of register: <%hhu>" DEFAULT, register_code);

    return END;
}


return_codes jump(processor_t* processor_p){

    assert(processor_p);

    unsigned char argument = processor_p->byte_code[processor_p->i_p - 1] & 0b1110'0000;

    size_t jump_index = *((size_t*) (processor_p->byte_code + processor_p->i_p));
    processor_p->i_p += sizeof(size_t);

    bool         isJump      = true    ;
    return_codes return_code = CONTINUE;

    printf("Argument: %X\n", argument);

    if(argument != JUMP_ARG_ABS){

        printf("POPs for jump\n");

        elem_type left_elem  = NAN;
        elem_type right_elem = NAN;

        stackPop(processor_p->computing_stack, &right_elem);
        stackPop(processor_p->computing_stack, &left_elem );

        if(IS_POISON(left_elem) || IS_POISON(right_elem)){

            return END;
        }

        isJump = isJumping(argument, left_elem, right_elem, &return_code);

    }

    if(isJump){

        processor_p->i_p = jump_index;
        ON_COMMANDS_DUMP(printf("Jump done on idx: " GREEN "%llu\n" DEFAULT , jump_index);)
    }

    return return_code;
}

bool isJumping(const unsigned char argument,
               const elem_type     left_elem,
               const elem_type     right_elem,
               return_codes* const return_code_p){

    assert(argument);
    assert(return_code_p);

    *return_code_p = CONTINUE;

    switch(argument){//Функция разбора аргумента

        case JUMP_ARG_A:

            if(! (left_elem > right_elem))
                return false;
            break;

        case JUMP_ARG_AE:

            if(! (left_elem >= right_elem))
                return false;
            break;

        case JUMP_ARG_B:

            if(! (left_elem < right_elem))
                return false;
            break;

        case JUMP_ARG_BE:

            if(! (left_elem <= right_elem))
                return false;
            break;

        case JUMP_ARG_E:

            if(! isZero(left_elem - right_elem))
                return false;
            break;

        case JUMP_ARG_NE:

            if(!! isZero(left_elem - right_elem))
                return false;
            break;

        default:

            printf(RED "ERROR: incorrect argument of JUMP in left 3 bits, jump argument: <%X>" DEFAULT, argument);
            *return_code_p = END;
            return false;
    }

    return true;
}

return_codes call(processor_t* processor_p){

    assert(processor_p);

    size_t ret_idx = processor_p->i_p + sizeof(size_t);

    elem_type ret_i_p_double = *((elem_type*)(&ret_idx));

    stackPush(processor_p->returns_stack, ret_i_p_double);

    processor_p->i_p = *((size_t*) (processor_p->byte_code + processor_p->i_p));

    return CONTINUE;
}

return_codes ret(processor_t* processor_p){

    assert(processor_p);

    elem_type ret_i_p_double = POISON;

    stackPop(processor_p->returns_stack, &ret_i_p_double);

    if(IS_POISON(ret_i_p_double)){

        return END;
    }

    processor_p->i_p = *((size_t*) (&ret_i_p_double));

    return CONTINUE;
}

return_codes in(processor_t* processor_p){

    assert(processor_p);

    elem_type new_elem = NAN;

    printf(YELLOW "Please, enter number:\n" DEFAULT);

    scanf("%lg", &new_elem);

    stackPush(processor_p->computing_stack, new_elem);

    return CONTINUE;
}

return_codes out(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;

    stackPop(processor_p->computing_stack, &a);

    if(IS_POISON(a)){

        return END;
    }

    printf(GREEN BOLD "OUT: %lg\n" DEFAULT, a);

    return CONTINUE;
}

return_codes hlt(processor_t* processor_p){

    assert(processor_p);

    return END;
}

#define BINARY_ARITHMETIC_OPERATION(func_name, operation)   \
                                                            \
return_codes func_name(processor_t* processor_p){           \
                                                            \
    assert(processor_p);                                    \
                                                            \
    elem_type a = NAN;                                      \
    elem_type b = NAN;                                      \
                                                            \
    stackPop(processor_p->computing_stack, &b);             \
    stackPop(processor_p->computing_stack, &a);             \
                                                            \
    if(IS_POISON(a) || IS_POISON(b)){                       \
                                                            \
        return END;                                         \
    }                                                       \
                                                            \
    stackPush(processor_p->computing_stack, a operation b); \
                                                            \
    return CONTINUE;                                        \
}

BINARY_ARITHMETIC_OPERATION(add, +)
BINARY_ARITHMETIC_OPERATION(sub, -)
BINARY_ARITHMETIC_OPERATION(mul, *)
BINARY_ARITHMETIC_OPERATION(div, /)

#undef BINARY_ARITHMETIC_OPERATION

return_codes sqrt(processor_t* processor_p){

    assert(processor_p);

    elem_type a = NAN;

    stackPop(processor_p->computing_stack, &a);

    if(IS_POISON(a)){

        return END;
    }

    stackPush(processor_p->computing_stack, pow(a, 0.5));

    return CONTINUE;
}

void endOfWorking(processor_t* processor_p){

    assert(processor_p);

    stackDelete(processor_p->computing_stack);

    free(processor_p->byte_code);

    printf("HLT\n");
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
    printf(VIOLET "\n---------------------------------------END_OF_DUMP------------------------------------\n\n" DEFAULT);
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

bool isZero(elem_type number){

    return (abs(number) < EPSILON) ? true : false;
}
