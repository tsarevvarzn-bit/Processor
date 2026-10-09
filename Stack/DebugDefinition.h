#define IS_COMMON_PROTECT 0
#define IS_CANARY_PROTECT 0
#define IS_HASH_PROTECT   0



#if IS_COMMON_PROTECT //-----------------------------------------------------------------------------------------------------------------------------

    #define ON_COMMON(...) __VA_ARGS__

    #define MY_ASSERT(str)                                                                                          \
                                                                                                                    \
    if(!(str)){                                                                                                     \
        printf("\nMy assertion failed: " #str ", file %s:%d, function: %s", __FILE_NAME__, __LINE__, __func__);     \
        abort();                                                                                                    \
    }

#else

    #define ON_COMMON(...)

    #define MY_ASSERT(str)

#endif

//Если нет канареек, data сразу указывает на первый элемент
//---------------------------------------------------------------------------------------------------------------------------------------------------

const size_t       TRUE_CANARY_DATA_LEFT   = 0xC0FE;   //TODO разные
const size_t       TRUE_CANARY_DATA_RIGHT  = 0x67DED76;//TODO разные
const size_t       TRUE_CANARY_STACK_LEFT  = 0x228322; //TODO разные
const size_t       TRUE_CANARY_STACK_RIGHT = 0xABCDEF; //TODO разные


#if IS_CANARY_PROTECT

    #define ON_CANARY(...) __VA_ARGS__

    #define CANARY_DATA_RIGHT(stack_p) *((size_t*) (stack_p->data + sizeof(TRUE_CANARY_DATA_LEFT) + sizeof(elem_type) * stack_p->capacity))
    #define CANARY_DATA_LEFT(stack_p)  *((size_t*) (stack_p->data))

    #define FIRST_ELEM_P(stack_p)      ((elem_type*)(stack_p->data + sizeof(TRUE_CANARY_DATA_LEFT)))

#else

    #define ON_CANARY(...)

    #define CANARY_DATA_LEFT(stack_p)
    #define CANARY_DATA_RIGHT(stack_p)

    #define FIRST_ELEM_P(stack_p)      ((elem_type*)(stack_p->data))

#endif


#if IS_HASH_PROTECT //-----------------------------------------------------------------------------------------------------------------------------

    #define ON_HASH(...) __VA_ARGS__

#else

    #define ON_HASH(...)

#endif


#if IS_HASH_PROTECT ||  IS_CANARY_PROTECT || IS_COMMON_PROTECT //------------------------------------------------------------------------------------------

    #define ON_DEBUG(...) __VA_ARGS__
    #define CHECK_STACK(stack_p, comment) checkStack(stack_p, __FILE_NAME__, __LINE__, __func__, comment) //TODO time

#else

    #define ON_DEBUG(...)
    #define CHECK_STACK(stack_p, comment)

#endif

struct stack_t{

    ON_CANARY (size_t     canary_left;)

               size_t     size;
               size_t     capacity;
               char*      data;

    ON_HASH   (size_t     data_hash;)
    ON_HASH   (size_t     struct_hash;)

    ON_CANARY (size_t     canary_right;)
};//TODO right canary pointer - macros
