void        printStack        (const stack_t*  const stack_p);
void        printBeautifulSpaces(size_t elem_index, size_t max_index);



void        printStack(const stack_t* const stack_p){

    MY_ASSERT(stack_p)

              printf("\nPrinting of stack " CYAN ON_COMMON("[%p]") DEFAULT ":\n\n" DEFAULT ON_COMMON(, stack_p));
    ON_CANARY(printf("\tcanary_left  "      CYAN ON_COMMON("[%p]") DEFAULT ": "   BLUE "0x%llX" DEFAULT ", true canary: " BLUE "0x%llX\n" DEFAULT ON_COMMON(, &stack_p->canary_left), stack_p->canary_left, TRUE_CANARY_STACK_LEFT);)
              printf("\tsize         "      CYAN ON_COMMON("[%p]") DEFAULT ": "  GREEN "%llu\n" DEFAULT, ON_COMMON(&stack_p->size,) stack_p->size);
              printf("\tcapacity     "      CYAN ON_COMMON("[%p]") DEFAULT ": " YELLOW "%llu\n" DEFAULT, ON_COMMON(&stack_p->capacity,) stack_p->capacity);
              printf("\tdata"      CYAN ON_COMMON("         [%p]") DEFAULT ":\n\n" DEFAULT ON_COMMON(,stack_p->data));

    if(stack_p->data){

        ON_CANARY(printf("\t left canary in data" ON_COMMON(CYAN "[%p]" DEFAULT)": " BLUE "0x%llX" DEFAULT ", true canary: " BLUE "0x%llX\n" DEFAULT,
        ON_COMMON(&CANARY_DATA_LEFT(stack_p),) CANARY_DATA_LEFT(stack_p), TRUE_CANARY_DATA_LEFT);)

        printf("\t elemets" CYAN ON_COMMON("            [%p]") DEFAULT ":\n" ON_COMMON(, FIRST_ELEM_P(stack_p)));

        for(size_t i = 0; i < stack_p->capacity ; i++){

            printf("\t\t");
            if(i < stack_p->size){

                printf(GREEN "*"  DEFAULT "[" YELLOW "%llu" DEFAULT "]" DEFAULT, i);
                printBeautifulSpaces(i, stack_p->capacity - 1);
                printf(" = " VIOLET);
                PRINT_ELEM(FIRST_ELEM_P(stack_p)[i]);
                printf(DEFAULT);

                ON_COMMON(
                if(IS_POISON(FIRST_ELEM_P(stack_p)[i]))
                    printf(YELLOW " - POISON" DEFAULT);
                )

                printf("\n");

            }else{

                printf(DEFAULT " [" DEFAULT "%llu" DEFAULT "]" DEFAULT, i);
                printBeautifulSpaces(i, stack_p->capacity - 1);
                printf(" = ");
                PRINT_ELEM(FIRST_ELEM_P(stack_p)[i]);

                ON_COMMON(
                if(!IS_POISON(FIRST_ELEM_P(stack_p)[i]))
                    printf(YELLOW " - NOT A POISON" DEFAULT);
                )

                printf("\n");
            }
        }

        ON_CANARY(printf("\t right canary in data" ON_COMMON(CYAN "[%p]" DEFAULT) ": " BLUE "0x%llX" DEFAULT ", true canary: " BLUE "0x%llX\n" DEFAULT,
        ON_COMMON(&CANARY_DATA_RIGHT(stack_p),) CANARY_DATA_RIGHT(stack_p), TRUE_CANARY_DATA_RIGHT);)
    }

    ON_HASH(  printf("\n\tdata_hash    " CYAN ON_COMMON("[%p]") DEFAULT ": " BLUE "0x%llX" DEFAULT " - correct" ON_COMMON(, &stack_p->data_hash),   stack_p->data_hash  );)
    ON_HASH(  printf("\n\tstruct_hash  " CYAN ON_COMMON("[%p]") DEFAULT ": " BLUE "0x%llX" DEFAULT " - correct" ON_COMMON(, &stack_p->struct_hash), stack_p->struct_hash);)

    ON_CANARY(printf("\n\tcanary_right " CYAN ON_COMMON("[%p]") DEFAULT ": " BLUE "0x%llX" DEFAULT ", true canary: " BLUE "0x%llX" DEFAULT ON_COMMON(, &stack_p->canary_right), stack_p->canary_right, TRUE_CANARY_STACK_RIGHT);)

    printf("\n");

    MY_ASSERT(stack_p->data)
}

void        printBeautifulSpaces(size_t elem_index, size_t max_index){

    MY_ASSERT(elem_index <= max_index)

    int size_log = 0;

    while(elem_index >= 10){

        elem_index /= 10;
        size_log++;
    }

    while(max_index >= 10){

        max_index /= 10;
        size_log--;

        if(size_log < 0){
            putchar(' ');
        }
    }
}
