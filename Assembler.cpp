#include "Assembler.h"



int main(const int argc, char* const* argv){

    char* out_file_name_p = NULL;
    char* in_file_name_p = NULL;

    processCMDArguments(argc, argv, &in_file_name_p, &out_file_name_p);

    int descr_in = open(in_file_name_p, _O_BINARY);
    int descr_out = open(out_file_name_p, _O_WRONLY);

    struct stat file_stats = {};
    fstat(descr_in, &file_stats);

    char* asm_code  = (char*) calloc(file_stats.st_size, sizeof(char));
    char* byte_code = (char*) calloc(file_stats.st_size, sizeof(char));

    read(descr_in, asm_code, file_stats.st_size); //Там \r и \n!!

    //printf("Len: %lu\n", file_stats.st_size);

    //write(descr, command, 1000);
    //printf("%s", code);

    char command[100]  = "";
    double argument = NAN;

    size_t next_str_idx = 0;
    int num_of_correct_arguments = 0;

    size_t byte_code_idx = 0;

    for(int asm_idx = 0; asm_idx < file_stats.st_size; asm_idx++){

        while(asm_code[asm_idx] != '\n' && asm_idx < file_stats.st_size - 1){

            asm_idx++;
        }

        asm_code[asm_idx] = '\0';

        num_of_correct_arguments = sscanf(asm_code + next_str_idx, "%s %lg", command, &argument);

        printf("From next_str_idx = <%llu>, detected command: <%s> with argument <%lg>, num of correct arguments of sscanf %d\n", next_str_idx, command, argument, num_of_correct_arguments);

        if(strcmp(command, "PUSH") == 0 && ! isnan(argument)){

            byte_code[byte_code_idx++] = PUSH;

            char* arg_byte = (char*) (&argument);

            for(int i = 0; i < 8; i++){

                byte_code[byte_code_idx++] = arg_byte[i];

            }

        }else if(strcmp(command, "ADD") == 0 && isnan(argument)){

            byte_code[byte_code_idx++] = ADD;

        }else if(strcmp(command, "SUB") == 0 && isnan(argument)){

            byte_code[byte_code_idx++] = SUB;

        }else if(strcmp(command, "OUT") == 0 && isnan(argument)){

            byte_code[byte_code_idx++] = OUT;

        }else if(strcmp(command, "HLT") == 0 && isnan(argument)){

            byte_code[byte_code_idx++] = HLT;

        }else{

            printf("INCORRECT COMMAND IN ASM_IDX = %llu\n", next_str_idx);
        }

        next_str_idx = asm_idx + 1;

        //Убрать потом
        argument = NAN;
        command[0]  = 'Q';
    }

    write(descr_out, byte_code, (unsigned int) byte_code_idx);

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

