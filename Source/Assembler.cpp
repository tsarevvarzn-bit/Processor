#include "..\Headers\Assembler.h"



int main(const int argc, char* const* argv){

    assembler_t assembler = {};

    if(startAssembler(&assembler, argc, argv) == END){

        return errno;
    }


    for(; assembler.num_of_completed_assemblies < 2; assembler.num_of_completed_assemblies++){

        if(assembleTheCode(&assembler) == END){

            return errno;
        }
    }

    write(assembler.descr_out, assembler.byte_code, (unsigned int) assembler.byte_code_idx);
}

return_codes startAssembler(assembler_t* assembler_p, const int argc, char* const* argv){

    assert(assembler_p);
    assert(argv);

    char* out_file_name_p = NULL;
    char* in_file_name_p  = NULL;

    processCMDArguments(argc, argv, &in_file_name_p, &out_file_name_p);

    int descr_in           = open(in_file_name_p,  _O_BINARY | _O_RDONLY);
    assembler_p->descr_out = open(out_file_name_p, _O_BINARY | _O_WRONLY | _O_CREAT | _O_TRUNC);

    if(descr_in == EOF){

        printf(RED "ERROR: there is no input file with name: <%s>\n", in_file_name_p);
        return END;
    }

    struct stat file_stats = {};
    fstat(descr_in, &file_stats);

    assembler_p->asm_code  = (char*) calloc(file_stats.st_size, sizeof(char));
    assembler_p->byte_code = (char*) calloc(file_stats.st_size, sizeof(char));

    read(descr_in, assembler_p->asm_code, file_stats.st_size);

    assembler_p->asm_code_idx   = 0;
    assembler_p->byte_code_idx  = 0;

    assembler_p->asm_code_len   = file_stats.st_size;
    assembler_p->byte_code_len  = file_stats.st_size;

    assembler_p->next_str_idx   = 0;
    assembler_p->num_of_command = 1;

    assembler_p->num_of_marks   = 0;



    return CONTINUE;
}



return_codes assembleTheCode(assembler_t* assembler_p){

    assert(assembler_p);

    assembler_p->asm_code_idx   = 0;
    assembler_p->byte_code_idx  = 0;

    assembler_p->next_str_idx   = 0;
    assembler_p->num_of_command = 1;

    while(assembler_p->asm_code[assembler_p->asm_code_idx] == ' '){

        assembler_p->asm_code_idx++;
    }

    assembler_p->next_str_idx = assembler_p->asm_code_idx;

    for(; assembler_p->asm_code_idx < assembler_p->asm_code_len; assembler_p->asm_code_idx++){

        while(assembler_p->asm_code[assembler_p->asm_code_idx] != '\n' &&
              assembler_p->asm_code[assembler_p->asm_code_idx] != '\0' &&
              assembler_p->asm_code_idx < assembler_p->asm_code_len - 1){

            assembler_p->asm_code_idx++;
        }

        assembler_p->asm_code[assembler_p->asm_code_idx++] = '\0';

        while(assembler_p->asm_code[assembler_p->asm_code_idx] == ' '){

            assembler_p->asm_code_idx++;
        }

        if(processNextCommand(assembler_p) == END){

            return END;
        }

        assembler_p->num_of_command++;
        assembler_p->next_str_idx = assembler_p->asm_code_idx;

    }

    return CONTINUE;
}

return_codes processNextCommand(assembler_t* assembler_p){

        assert(assembler_p);

        checkAndResizeUpByteCode(assembler_p);

        char command[MAX_COMMAND_LEN]  = ""      ;
        bool isStrProcessed            = false   ;
        return_codes command_return    = CONTINUE;

        sscanf(assembler_p->asm_code + assembler_p->next_str_idx, "%s", command);

        printf("From str: %llu, asm code idx: %llu detected command: <%s>\n",
               assembler_p->num_of_command, assembler_p->next_str_idx, command);

        for(size_t i = 0; i < (sizeof(commands) / sizeof(commands[0])); i++){

            if(strncmp(commands[i].name, command, MAX_COMMAND_LEN) == 0){

                if(commands[i].function != NULL){

                    command_return = commands[i].function(assembler_p);

                }else{

                    assert(commands[i].code != ZERO);
                    assembler_p->byte_code[assembler_p->byte_code_idx++] = commands[i].code;

                }

                isStrProcessed = true;

                break;
            }
        }

        printf("isStrProcessed: %d, command: <%s>\n", isStrProcessed, command);

        if(!isStrProcessed && !isNotCommandStrCorrect(assembler_p, command)){

            printf(RED "Syntax error on %llu str\n" DEFAULT, assembler_p->num_of_command);
            return END;
        }

        asmDump(assembler_p);

        return command_return;
}

bool isNotCommandStrCorrect(assembler_t* assembler_p, const char* const command){

    assert(assembler_p);
    assert(command);

    if(command[0] == '\0'){

        return true;
    }

    if(command[0] == '/' && command[1] == '/'){

        return true;
    }

    if(command[0] == ':'){

        processMark(assembler_p);
        return true;
    }

    return false;
}

return_codes processPush(assembler_t* assembler_p){

    assert(assembler_p);

    pushPopConfig_t pushConfig = {

    .arg_number_imm  = NAN,
    .arg_reg_code    = 0,
    .command         = PUSH,
    };

    if(parsePushPopArgument(&pushConfig, assembler_p, 4) == END){

        return END;
    }

    assembler_p->byte_code[assembler_p->byte_code_idx++] = pushConfig.command;

    if(pushConfig.command & PUSH_ARG_REG){

        assembler_p->byte_code[assembler_p->byte_code_idx++] = pushConfig.arg_reg_code;
    }

    if(pushConfig.command & PUSH_ARG_IMMED_CONST){

        const char* arg_byte = (const char*) (&(pushConfig.arg_number_imm));

        for(size_t i = 0; i < sizeof(double); i++){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = arg_byte[i];

        }
    }

    printf("Arg num imm: %lg, arg reg code: %c, summary command: %X\n",
    pushConfig.arg_number_imm, pushConfig.arg_reg_code, pushConfig.command);

    return CONTINUE;
}

return_codes parsePushPopArgument(pushPopConfig_t* pushPopConfig_p, assembler_t* assembler_p, const int len_of_command){

    assert(pushPopConfig_p);
    assert(assembler_p);

    char first_arg_str [MAX_COMMAND_LEN]  = "";
    char second_arg_str[MAX_COMMAND_LEN]  = "";

    int num_of_correct_arguments = sscanf(assembler_p->asm_code + assembler_p->next_str_idx + len_of_command + 1,
    "%[^] +\r]%*[^-0-9]%[^] \r]", first_arg_str, second_arg_str);

    printf(", with %d arguments: <%s>, <%s>\n\n", num_of_correct_arguments, first_arg_str, second_arg_str);

    if(num_of_correct_arguments == 1){

        return parsePushPopWithOneArgument(pushPopConfig_p, assembler_p, first_arg_str, second_arg_str);

    }else if(num_of_correct_arguments == 2){

        return parsePushPopWithTwoArguments(pushPopConfig_p, assembler_p, first_arg_str, second_arg_str);

    }else{

        printf(RED "ERROR: incorrect input for push in %llu str: there is no arguments\n" DEFAULT, assembler_p->num_of_command);
        return END;
    }
}

return_codes parsePushPopWithOneArgument(pushPopConfig_t* pushPopConfig_p, assembler_t* assembler_p, const char* const first_arg_str, const char* const second_arg_str){

    assert(pushPopConfig_p);
    assert(assembler_p);
    assert(first_arg_str);
    assert(second_arg_str);

    char shift_for_first_arg_str = 0;

    if(first_arg_str[0] == '['){

        pushPopConfig_p->command = pushPopConfig_p->command | PUSH_ARG_RAM;
        shift_for_first_arg_str = 1;
    }

    if(first_arg_str[shift_for_first_arg_str + 1] == 'X'){

        pushPopConfig_p->command = pushPopConfig_p->command | PUSH_ARG_REG;

        if(processRegister(assembler_p, first_arg_str + shift_for_first_arg_str, &pushPopConfig_p->arg_reg_code) == END){

            return END;
        }

    }else{

        pushPopConfig_p->command = pushPopConfig_p->command | PUSH_ARG_IMMED_CONST;

        if(sscanf(first_arg_str + shift_for_first_arg_str, "%lg", &(pushPopConfig_p->arg_number_imm)) != 1){

            printf(RED "ERROR: incorrect input for push in %llu str, incorrect argument: <%s>\n" DEFAULT,
            assembler_p->num_of_command, first_arg_str);
            return END;
        }
    }

    return CONTINUE;
}

return_codes parsePushPopWithTwoArguments(pushPopConfig_t* pushPopConfig_p, assembler_t* assembler_p, const char* const first_arg_str, const char* const second_arg_str){

    assert(pushPopConfig_p);
    assert(assembler_p);
    assert(first_arg_str);
    assert(second_arg_str);

    char shift_for_first_arg_str = 0;

    pushPopConfig_p->command = pushPopConfig_p->command | PUSH_ARG_REG | PUSH_ARG_IMMED_CONST;

    if(first_arg_str[0] == '['){

        pushPopConfig_p->command = pushPopConfig_p->command | PUSH_ARG_RAM;
        shift_for_first_arg_str = 1;
    }

    if(processRegister(assembler_p, first_arg_str + shift_for_first_arg_str, &pushPopConfig_p->arg_reg_code) == END){

        return END;
    }

    if(sscanf(second_arg_str, "%lg", &(pushPopConfig_p->arg_number_imm)) != 1){

        printf(RED "ERROR: incorrect input for push in %llu str, incorrect second argument: <%s>\n" DEFAULT,
        assembler_p->num_of_command, second_arg_str);
        return END;
    }

    return CONTINUE;
}

return_codes processPop(assembler_t* assembler_p){

    assert(assembler_p);


    pushPopConfig_t pushConfig = {

    .arg_number_imm  = NAN,
    .arg_reg_code    = 0,
    .command         = POP,
    };

    if(parsePushPopArgument(&pushConfig, assembler_p, 3) == END){

        return END;
    }

    assembler_p->byte_code[assembler_p->byte_code_idx++] = pushConfig.command;

    if(pushConfig.command & PUSH_ARG_REG){

        assembler_p->byte_code[assembler_p->byte_code_idx++] = pushConfig.arg_reg_code;
    }

    if(pushConfig.command & PUSH_ARG_IMMED_CONST){

        const char* arg_byte = (const char*) (&(pushConfig.arg_number_imm));

        for(size_t i = 0; i < sizeof(double); i++){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = arg_byte[i];
        }
    }

    printf("Arg num imm: %lg, arg reg code: %c, summary command: %X",
    pushConfig.arg_number_imm, pushConfig.arg_reg_code, pushConfig.command);

    return CONTINUE;
}

return_codes processRegister(assembler_t* assembler_p, const char* const name_of_reg, unsigned char* const reg_code_p){

    assert(assembler_p);
    assert(name_of_reg);
    assert(reg_code_p);

    if(      strcmp(name_of_reg, "AX") == 0){

        *reg_code_p = AX;

    }else if(strcmp(name_of_reg, "BX") == 0){

        *reg_code_p = BX;

    }else if(strcmp(name_of_reg, "CX") == 0){

        *reg_code_p = CX;

    }else if(strcmp(name_of_reg, "DX") == 0){

        *reg_code_p = DX;

    }else{

        printf(RED "INCORRECT COMMAND IN %llu string, asm_code_idx = %llu, POPr have <%s> - incorrect argument\n" DEFAULT,
        assembler_p->num_of_command, assembler_p->next_str_idx, name_of_reg);
        return END;
    }

    return CONTINUE;
}

#define CONDITIONAL_JUMP(func_name, code, len_of_command)                           \
                                                                                    \
return_codes func_name(assembler_t* assembler_p){                                   \
                                                                                    \
    assert(assembler_p);                                                            \
                                                                                    \
    unsigned char command = JUMP;                                                   \
    assembler_p->byte_code[assembler_p->byte_code_idx++] = command | code;          \
                                                                                    \
    return decryptMark(assembler_p, assembler_p->next_str_idx + len_of_command);    \
}

CONDITIONAL_JUMP(processJump, JUMP_ARG_ABS, 4)
CONDITIONAL_JUMP(processJA,   JUMP_ARG_A,   2)
CONDITIONAL_JUMP(processJAE,  JUMP_ARG_AE,  3)
CONDITIONAL_JUMP(processJB,   JUMP_ARG_B,   2)
CONDITIONAL_JUMP(processJBE,  JUMP_ARG_BE,  3)
CONDITIONAL_JUMP(processJE,   JUMP_ARG_E,   2)
CONDITIONAL_JUMP(processJNE,  JUMP_ARG_NE,  3)

#undef CONDITIONAL_JUMP

return_codes processCall(assembler_t* assembler_p){

    assert(assembler_p);

    assembler_p->byte_code[assembler_p->byte_code_idx++] = CALL;

    return decryptMark(assembler_p, assembler_p->next_str_idx + 4);
}

return_codes processDelay(assembler_t* assembler_p){

    assert(assembler_p);

    assembler_p->byte_code[assembler_p->byte_code_idx++] = DELAY;

    double delay_time = 0;

    if(sscanf(assembler_p->asm_code + assembler_p->next_str_idx + 5, "%lg", &delay_time) != 1){

        printf(RED "ERROR: incorrect argument of delay on %llu str\n", assembler_p->num_of_command);
        return END;
    }

    char* arg_byte = (char*) (&delay_time);

    for(size_t i = 0; i < sizeof(double); i++){

        assembler_p->byte_code[assembler_p->byte_code_idx++] = arg_byte[i];
    }

    return CONTINUE;
}

return_codes decryptMark(assembler_t* assembler_p, size_t mark_idx){

    assert(assembler_p);

    size_t jump_index = 0;
    char   mark_name[MAX_MARK_NAME_LEN] = "";

    if(assembler_p->asm_code[mark_idx] != ' '){

        printf(RED "INCORRECT COMMAND IN %llu string, asm_code_idx = %llu, J... have <%c> after P\n" DEFAULT,
        assembler_p->num_of_command, assembler_p->next_str_idx, assembler_p->asm_code[assembler_p->next_str_idx + 4]);

        return END;
    }

    if(assembler_p->asm_code[mark_idx + 1] == ':'){

        sscanf(assembler_p->asm_code + mark_idx + 2, "%s", mark_name);

        if(! findMark(assembler_p, mark_name, &jump_index) && assembler_p->num_of_completed_assemblies >= 1){

            printf(RED "ERROR: non-defined mark: <%s>, on str: %llu", assembler_p->asm_code + mark_idx + 2, assembler_p->num_of_command);
            return END;
        }

    }else if(sscanf(assembler_p->asm_code + mark_idx + 2, "%llu", &jump_index) != 1){

        printf(RED "INCORRECT COMMAND IN %llu string, asm_code_idx = %llu, J... have <%llu> jump_index argument\n" DEFAULT,
        assembler_p->num_of_command, assembler_p->next_str_idx, jump_index);
        return END;
    }

    char* arg_byte = (char*) (&jump_index);

    for(size_t i = 0; i < sizeof(size_t); i++){

        assembler_p->byte_code[assembler_p->byte_code_idx++] = arg_byte[i];

    }

    return CONTINUE;
}

bool findMark(assembler_t* assembler_p, const char* const name, size_t* mark_idx_p){

    assert(assembler_p);
    assert(name);

    for(int i = 0; i < assembler_p->num_of_marks; i++){

        if(strncmp(name, assembler_p->marks[i].name, MAX_MARK_NAME_LEN) == 0){

            *mark_idx_p = assembler_p->marks[i].idx;
            printf("findMark() find mark with name <%s> and value <%llu>\n", name, *mark_idx_p);
            return true;
        }
    }

    return false;
}

return_codes processMark(assembler_t* assembler_p){

    assert(assembler_p);

    char   mark_name[MAX_MARK_NAME_LEN] = "";
    size_t mark_value                   = DEFAULT_MARK_VALUE;

    sscanf(assembler_p->asm_code + assembler_p->next_str_idx + 1, "%s", mark_name);

    if(findMark(assembler_p, mark_name, &mark_value) == true){

        return CONTINUE;
    }

    if(assembler_p->num_of_marks >= MAX_MARK_NAME_LEN - 1){

        printf(RED "ERROR: to many marks!\n" DEFAULT);
        return END;
    }

    strcpy(assembler_p->marks[assembler_p->num_of_marks].name, mark_name);
    assembler_p->marks[assembler_p->num_of_marks].idx    = assembler_p->byte_code_idx;

    printf(GREEN "DEFINED NEW MARK: name <%s>, value: <%llu>" DEFAULT, mark_name, assembler_p->byte_code_idx);
    assembler_p->num_of_marks++;

    return CONTINUE;
}

return_codes checkAndResizeUpByteCode(assembler_t* assembler_p){

    assert(assembler_p);

    if(assembler_p->byte_code_idx >= assembler_p->byte_code_len - sizeof(char) - sizeof(double)){

        if((assembler_p->byte_code = (char*) realloc(assembler_p->byte_code, assembler_p->byte_code_len * 2)) == NULL){

            printf(RED "ERROR: there is no enough memory, can't resize up assembler_p->byte_code, assembler_p->byte_code_len: %llu" DEFAULT, assembler_p->byte_code_len);

            return END;

        }else{

            printf("Correct reallocation of byte_code\n");
        }
    }

    return CONTINUE;
}

void asmDump(assembler_t* assembler_p){

    assert(assembler_p);

    printf(BLUE "\nAssembler struct dump:\n\n" DEFAULT);
    printf("Asm code str: " GREEN "%llu" DEFAULT ", str idx: " GREEN "%llu" DEFAULT ", idx: " GREEN "%llu\n" DEFAULT,
    assembler_p->num_of_command, assembler_p->next_str_idx, assembler_p->asm_code_idx);
    printf("Byte code idx:" GREEN " %llu\n" DEFAULT, assembler_p->byte_code_idx);
    printf("Num of marks: " GREEN "%u\n" DEFAULT, assembler_p->num_of_marks);
    printf("Marks:\n" DEFAULT);

    for(int i = 0; i < assembler_p->num_of_marks; i++){

        printf("\tName: <" YELLOW "%s" DEFAULT ">, idx: <" GREEN "%llu" DEFAULT ">\n", assembler_p->marks[i].name, assembler_p->marks[i].idx);
    }

    printf("\n");
}

return_codes processCMDArguments(const int argc, char* const * argv, char** const name_from_p, char** const name_to_p){

    assert(argv);
    assert(name_from_p);
    assert(name_to_p);

    int getopt_out = 0;

    while ((getopt_out = getopt(argc, argv, "f:t:")) != -1){

        switch (getopt_out){

            case 'f':
                *name_from_p = optarg;
                break;

            case 't':
                *name_to_p = optarg;
                break;

            default:
                break;
        }
    }

    if(*name_from_p == NULL || *name_to_p == NULL){

        printf("ERROR: please, enter correct CMD arguments: -f name_of_input_file.txt -t name_of_output_file.txt\n");

        return END;
    }

    return CONTINUE;
}
