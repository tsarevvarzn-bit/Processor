#include "Assember.h"

int main(const int argc, char* const* argv){

    assert(argc == 2)


}

void         processCMDArguments(const int argc, char* const * argv, char** const name_from_p, char** const name_to_p){

    assert(argv);
    assert(name_from_p);
    assert(name_to_p);

    int getopt_out = 0;

    while ((getopt_out = getopt(argc, argv, "f:t:")) != -1)
    {
        switch (getopt_out)
        {
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

        printf(RED "ERROR: please, enter correct CMD arguments: -f name_of_input_file.txt -t name_of_output_file.txt\n" DEFAULT);
        exit(EXIT_FAILURE);
    }

}
