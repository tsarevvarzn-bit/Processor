#define CHAR_TYPE 0
#define DOUBLE_TYPE 1

#define ELEM_TYPE DOUBLE_TYPE

#if ELEM_TYPE == CHAR_TYPE

    #define POISON '@'

    #define IS_POISON(elem) ((elem) == POISON)

    typedef char elem_type;

    #define PRINT_ELEM(x) printf("%c", x)

    #define SCAN_ELEM(x) getchar(); scanf("%c", x)

#elif ELEM_TYPE == DOUBLE_TYPE

    #define POISON NAN

    #define IS_POISON(elem) isnan(elem)

    typedef double elem_type;

    #define PRINT_ELEM(x) printf("%lg", x)

    #define SCAN_ELEM(x) scanf("%lg", x)

#endif
