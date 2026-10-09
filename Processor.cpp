#include "Processor.h"



int main(const int argc, char* const* argv)
{

    stack_t* stack_p = NULL;
    createAndInitStack(&stack_p, DEFAULT_STACK_SIZE);

    char* out_file_name_p = NULL;

    processCMDArguments(argc, argv, &out_file_name_p);

    int descr = open(out_file_name_p, _O_BINARY);

    struct stat file_stats = {};
    fstat(descr, &file_stats);

    char* code = (char*) calloc(file_stats.st_size, sizeof(char));

    read(descr, code, file_stats.st_size); //Там \r и \n!!

    //printf("Len: %lu\n", file_stats.st_size);

    //write(descr, command, 1000);
    //printf("%s", code);

    unsigned char command = 255;
    elem_type argument = NAN;

    size_t next_str_idx = 0;

    for(int i = 0; i < file_stats.st_size; i++)
    {
        while(code[i] != '\n' && i < file_stats.st_size - 1)
        {
            i++;
        }

        code[i] = '\0';

        sscanf(code + next_str_idx, "%hhu %lg", &command, &argument);

        printf("From idx = <%llu>, next_str_idx, ", next_str_idx);
        next_str_idx = i + 1;

        printf("Detected command: <%hhu> with argument <%lg>\n", command, argument);

        switch (command){

            case add:

                stackPush(stack_p, a);
                break;

            case sub:

                sub(stack_p);
                break;

            case out:

                out(stack_p);
                break;

            case end:

                end(stack_p);
                break;

            default:

                printf(RED "INCORRECT COMMAND" DEFAULT);
                end(stack_p);
                break;
        }

        //Убрать потом
        argument = NAN;
        command  = 255;
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

void sub(stack_t* stack_p){

    assert(stack_p);

    elem_type a = NAN;
    elem_type b = NAN;

    stackPop(stack_p, &b);
    stackPop(stack_p, &a);

    stackPush(stack_p, a - b);
}

void add(stack_t* stack_p){

    assert(stack_p);

    elem_type a = NAN;
    elem_type b = NAN;

    stackPop(stack_p, &b);
    stackPop(stack_p, &a);

    stackPush(stack_p, a + b);
}

void out(stack_t* stack_p){

    assert(stack_p);

    elem_type a = NAN;

    stackPop(stack_p, &a);

    printf("%lg", a);
}

void end(stack_t* stack_p){

    assert(stack_p);

    stackDelete(stack_p);

    exit(errno);
}
