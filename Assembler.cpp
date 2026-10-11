#include "Assembler.h"



int main(const int argc, char* const* argv){

    assembler_t assembler = {};
    startAssembler(&assembler, argc, argv);

    char command[100]  = "";
    double argument    = NAN;
    int num_of_correct_arguments = 0;

    for(; assembler.asm_code_idx < assembler.asm_code_len; assembler.asm_code_idx++){

        while(assembler.asm_code[assembler.asm_code_idx] != '\n' && assembler.asm_code_idx < assembler.asm_code_len - 1){

            assembler.asm_code_idx++;
        }

        assembler.asm_code[assembler.asm_code_idx] = '\0';

        num_of_correct_arguments = sscanf(assembler.asm_code + assembler.next_str_idx, "%s %lg", command, &argument);

        processTheCommand(&assembler, command, argument);

        printf("From next_str_idx = <%llu>, detected command: <%s> with argument <%lg>, num of correct arguments of sscanf %d\n", assembler.next_str_idx, command, argument, num_of_correct_arguments);

        assembler.num_of_command++;
        assembler.next_str_idx = assembler.asm_code_idx + 1;

        argument = NAN;
        command[0]  = 'Q';
    }

    write(assembler.descr_out, assembler.byte_code, (unsigned int) assembler.byte_code_idx);

}

void startAssembler(assembler_t* assembler_p, const int argc, char* const* argv){

    assert(assembler_p);
    assert(argv);

    char* out_file_name_p = NULL;
    char* in_file_name_p  = NULL;

    processCMDArguments(argc, argv, &in_file_name_p, &out_file_name_p);

    int descr_in           = open(in_file_name_p, _O_BINARY);
    assembler_p->descr_out = open(out_file_name_p, _O_WRONLY);

    struct stat file_stats = {};
    fstat(descr_in, &file_stats);

    assembler_p->asm_code  = (char*) calloc(file_stats.st_size, sizeof(char));
    assembler_p->byte_code = (char*) calloc(file_stats.st_size, sizeof(char));//TODO динамическое расширение

    read(descr_in, assembler_p->asm_code, file_stats.st_size);

    assembler_p->asm_code_idx  = 0;
    assembler_p->byte_code_idx = 0;

    assembler_p->asm_code_len  = file_stats.st_size;
    assembler_p->byte_code_len = file_stats.st_size;

    assembler_p->next_str_idx   = 0;
    assembler_p->num_of_command = 1;
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
        exit(EXIT_FAILURE);
    }

}

void processTheCommand(assembler_t* assembler_p, const char* const command, const double argument){

        if(strcmp(command, "PUSH") == 0 && ! isnan(argument)){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = PUSH;

            const char* arg_byte = (const char*) (&argument);

            for(int i = 0; i < 8; i++){

                assembler_p->byte_code[assembler_p->byte_code_idx++] = arg_byte[i];

            }

        }else if(strcmp(command, "ADD") == 0 && isnan(argument)){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = ADD;

        }else if(strcmp(command, "SUB") == 0 && isnan(argument)){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = SUB;

        }else if(strcmp(command, "OUT") == 0 && isnan(argument)){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = OUT;

        }else if(strcmp(command, "HLT") == 0 && isnan(argument)){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = HLT;

        }else if(strcmp(command, "MUL") == 0 && isnan(argument)){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = MUL;

        }else if(strcmp(command, "DIV") == 0 && isnan(argument)){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = DIV;

        }else if(strcmp(command, "SQRT") == 0 && isnan(argument)){

            assembler_p->byte_code[assembler_p->byte_code_idx++] = SQRT;

        }else{

            printf(RED "INCORRECT COMMAND IN %llu string, asm_code_idx = %llu\n" DEFAULT, assembler_p->num_of_command, assembler_p->next_str_idx);
            exit(0);
        }
}

