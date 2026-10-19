#include "Assembler.h"



int main(const int argc, char* const* argv){

    //TODO logger file with txt byte code
    assembler_t assembler = {};
    startAssembler(&assembler, argc, argv);

    for(; assembler.num_of_completed_assemblies < 2; assembler.num_of_completed_assemblies++){

        assembleTheCode(&assembler);
    }

    write(assembler.descr_out, assembler.byte_code, (unsigned int) assembler.byte_code_idx);
}

void startAssembler(assembler_t* assembler_p, const int argc, char* const* argv){

    assert(assembler_p);
    assert(argv);

    char* out_file_name_p = NULL;
    char* in_file_name_p  = NULL;

    processCMDArguments(argc, argv, &in_file_name_p, &out_file_name_p);

    int descr_in           = open(in_file_name_p,  _O_BINARY | _O_RDONLY);
    assembler_p->descr_out = open(out_file_name_p, _O_BINARY | _O_WRONLY | _O_CREAT | _O_TRUNC);

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
}



void assembleTheCode(assembler_t* assembler_p){

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

        processNextCommand(assembler_p);

        assembler_p->num_of_command++;
        assembler_p->next_str_idx = assembler_p->asm_code_idx;

    }
}

void processNextCommand(assembler_t* assembler_p){

        assert(assembler_p);

        checkAndResizeUpByteCode(assembler_p);

        char command[MAX_COMMAND_LEN]  = "";

        sscanf(assembler_p->asm_code + assembler_p->next_str_idx, "%s", command);

        printf("From next_str_idx = <%llu>, detected command: <%s>\n", assembler_p->next_str_idx, command);

        //TODO цикл strncmp шек на массиве структур имя + номер + количество аргументов(JUMP и PUSH)

        if(strcmp(command, "PUSH") == 0){

            processPush(assembler_p);

        }else if(strcmp(command, "POP") == 0){

            processPop(assembler_p);

        }else if(command[0] == 'J'){

            processJumps(assembler_p, command);

        }else if(strcmp(command, "CALL") == 0){

            processCall(assembler_p);

        }else if(strcmp(command, "RET") == 0){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = RET;

        }else if(strcmp(command, "OUT") == 0){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = OUT;

        }else if(strcmp(command, "HLT") == 0){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = HLT;

        }else if(strcmp(command, "ADD") == 0){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = ADD;

        }else if(strcmp(command, "SUB") == 0){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = SUB;

        }else if(strcmp(command, "MUL") == 0){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = MUL;

        }else if(strcmp(command, "DIV") == 0){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = DIV;

        }else if(strcmp(command, "SQRT") == 0){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = SQRT;

        }else if(command[0] == ':'){

            processMark(assembler_p);

        }else if(! (command[0] == '\0' || (command[0] == '/' && command[1] == '/'))){

            printf(RED "INCORRECT COMMAND IN %llu string, asm_code_idx = %llu\n" DEFAULT,
            assembler_p->num_of_command, assembler_p->next_str_idx);
            exit(0);
        }
        asmDump(assembler_p);
}

void processPush(assembler_t* assembler_p){

    assert(assembler_p);


    char str_argument[MAX_COMMAND_LEN]  = "";
    double arg = NAN;

    sscanf(assembler_p->asm_code + assembler_p->next_str_idx + 4, "%s", str_argument);

    printf(", with argument: <%s>\n\n", str_argument);

    unsigned char command = PUSH;

    if(str_argument[1] == 'X'){

        command = command | PUSH_ARG_REG;
        assembler_p->byte_code[assembler_p->byte_code_idx++] = command;

        if(      strcmp(str_argument, "AX") == 0){//Декодирование в отдельную функцию

            assembler_p->byte_code[assembler_p->byte_code_idx++] = AX;

        }else if(strcmp(str_argument, "BX") == 0){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = BX;

        }else if(strcmp(str_argument, "CX") == 0){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = CX;

        }else if(strcmp(str_argument, "DX") == 0){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = DX;

        }else{

            printf(RED "INCORRECT COMMAND IN %llu string, asm_code_idx = %llu, PUSHr have <%s> - incorrect argument\n" DEFAULT,
            assembler_p->num_of_command, assembler_p->next_str_idx, str_argument);
            exit(0);
        }

    }else{

        command = command | PUSH_ARG_IMMED_CONST;
        assembler_p->byte_code[assembler_p->byte_code_idx++] = command;

        sscanf(assembler_p->asm_code + assembler_p->next_str_idx + 4, "%lg", &arg);

        if(isnan(arg)){

            printf(RED "INCORRECT COMMAND IN %llu string, asm_code_idx = %llu, PUSH have NAN argument\n" DEFAULT,
            assembler_p->num_of_command, assembler_p->next_str_idx);
            exit(0);

        }else{

            const char* arg_byte = (const char*) (&arg);

            for(size_t i = 0; i < sizeof(double); i++){

                assembler_p->byte_code[assembler_p->byte_code_idx++] = arg_byte[i];

            }
        }
    }
}

void processPop(assembler_t* assembler_p){

    assert(assembler_p);


    char str_argument[100]  = "";

    sscanf(assembler_p->asm_code + assembler_p->next_str_idx + 4, "%s", str_argument);

    printf(", with argument: <%s>\n\n", str_argument);

    assembler_p->byte_code[assembler_p->byte_code_idx++] = POPr;

    if(      strcmp(str_argument, "AX") == 0){

        assembler_p->byte_code[assembler_p->byte_code_idx++] = AX;

    }else if(strcmp(str_argument, "BX") == 0){

        assembler_p->byte_code[assembler_p->byte_code_idx++] = BX;

    }else if(strcmp(str_argument, "CX") == 0){

        assembler_p->byte_code[assembler_p->byte_code_idx++] = CX;

    }else if(strcmp(str_argument, "DX") == 0){

        assembler_p->byte_code[assembler_p->byte_code_idx++] = DX;

    }else{

        printf(RED "INCORRECT COMMAND IN %llu string, asm_code_idx = %llu, POPr have <%s> - incorrect argument\n" DEFAULT,
        assembler_p->num_of_command, assembler_p->next_str_idx, str_argument);
        exit(0);
    }
}

void processJumps(assembler_t* assembler_p, const char* const command_name){

    assert(assembler_p);
    assert(command_name);

    unsigned char len_of_command = 0;
    unsigned char command        = JUMP;

    if      (strcmp(command_name, "JUMP" ) == 0){//TODO цикл по массиву комманд

        command = command | JUMP_ARG_ABS;
        len_of_command = 4;

    }else if(strcmp(command_name, "JA") == 0){

        command = command | JUMP_ARG_A;
        len_of_command = 2;

    }else if(strcmp(command_name, "JAE") == 0){

        command = command | JUMP_ARG_AE;
        len_of_command = 3;

    }else if(strcmp(command_name, "JB" ) == 0){

        command = command | JUMP_ARG_B;
        len_of_command = 2;

    }else if(strcmp(command_name, "JBE") == 0){

        command = command | JUMP_ARG_BE;
        len_of_command = 3;

    }else if(strcmp(command_name, "JE" ) == 0){

        command = command | JUMP_ARG_E;
        len_of_command = 2;

    }else if(strcmp(command_name, "JNE") == 0){

        command = command | JUMP_ARG_NE;
        len_of_command = 3;

    }else{

        printf(RED "INCORRECT COMMAND IN %llu string, asm_code_idx = %llu, J.. incorrect version: <%s>\n" DEFAULT,
        assembler_p->num_of_command, assembler_p->next_str_idx, command_name);
        exit(0);//!!!!!!!
    }

    assembler_p->byte_code[assembler_p->byte_code_idx++] = command;

    decryptMark(assembler_p, assembler_p->next_str_idx + len_of_command);
}

void processCall(assembler_t* assembler_p){

    assert(assembler_p);

    assembler_p->byte_code[assembler_p->byte_code_idx++] = CALL;

    decryptMark(assembler_p, assembler_p->next_str_idx + 4);
}

void decryptMark(assembler_t* assembler_p, size_t mark_idx){

    assert(assembler_p);

    size_t jump_index = 0;
    char   mark_name[MAX_MARK_NAME_LEN] = "";

    if(assembler_p->asm_code[mark_idx] != ' '){

        printf(RED "INCORRECT COMMAND IN %llu string, asm_code_idx = %llu, J... have <%c> after P\n" DEFAULT,
        assembler_p->num_of_command, assembler_p->next_str_idx, assembler_p->asm_code[assembler_p->next_str_idx + 4]);

        exit(errno);
    }

    if(assembler_p->asm_code[mark_idx + 1] == ':'){

        sscanf(assembler_p->asm_code + mark_idx + 2, "%s", mark_name);

        if(! findMark(assembler_p, mark_name, &jump_index) && assembler_p->num_of_completed_assemblies >= 1){

            printf(RED "ERROR: non-defined mark: <%s>, on str: %llu", assembler_p->asm_code + mark_idx + 2, assembler_p->num_of_command);
            exit(errno);
        }

    }else if(sscanf(assembler_p->asm_code + mark_idx + 2, "%llu", &jump_index) != 1){

        printf(RED "INCORRECT COMMAND IN %llu string, asm_code_idx = %llu, J... have <%llu> jump_index argument\n" DEFAULT,
        assembler_p->num_of_command, assembler_p->next_str_idx, jump_index);
        exit(errno);
    }

    char* arg_byte = (char*) (&jump_index);

    for(size_t i = 0; i < sizeof(size_t); i++){

        assembler_p->byte_code[assembler_p->byte_code_idx++] = arg_byte[i];

    }
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

void processMark(assembler_t* assembler_p){

    assert(assembler_p);

    char   mark_name[MAX_MARK_NAME_LEN] = "";
    size_t mark_value                   = DEFAULT_MARK_VALUE;

    sscanf(assembler_p->asm_code + assembler_p->next_str_idx + 1, "%s", mark_name);

    if(findMark(assembler_p, mark_name, &mark_value) == true){

        return;
    }

    if(assembler_p->num_of_marks >= MAX_MARK_NAME_LEN - 1){

        printf(RED "ERROR: to many marks!\n" DEFAULT);
        exit(errno);
    }

    strcpy(assembler_p->marks[assembler_p->num_of_marks].name, mark_name);
    assembler_p->marks[assembler_p->num_of_marks].idx    = assembler_p->byte_code_idx;

    printf(GREEN "DEFINED NEW MARK: name <%s>, value: <%llu>" DEFAULT, mark_name, assembler_p->byte_code_idx);
    assembler_p->num_of_marks++;
}

void checkAndResizeUpByteCode(assembler_t* assembler_p){

    assert(assembler_p);

    if(assembler_p->byte_code_idx >= assembler_p->byte_code_len - sizeof(char) - sizeof(double)){

        if((assembler_p->byte_code = (char*) realloc(assembler_p->byte_code, assembler_p->byte_code_len * 2)) == NULL){

            printf(RED "ERROR: there is no enough memory, can't resize up assembler_p->byte_code, assembler_p->byte_code_len: %llu" DEFAULT, assembler_p->byte_code_len);
            exit(errno);
        }else{

            printf("Correct reallocation of byte_code\n");
        }
    }
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

void processCMDArguments(const int argc, char* const * argv, char** const name_from_p, char** const name_to_p){

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
        exit(EXIT_FAILURE);//Надо убрать
    }

}


