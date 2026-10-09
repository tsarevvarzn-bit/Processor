#include "Common.h"
#include "TypeDefinition.h"
#include "DebugDefinition.h"
#include "InputOutput.cpp"
#include "Protect.cpp"

code_errors createAndInitStack(stack_t** const stack_p, const size_t capacity);
void        stackInit         (stack_t*  const stack_p, const size_t capacity);
code_errors stackPush         (stack_t*  const stack_p, const elem_type new_elem);
code_errors stackPop          (stack_t*  const stack_p, elem_type* const last_elem_p);
void        stackDelete       (stack_t*  const stack_p);

code_errors stackResizeUp     (stack_t*  const stack_p);
code_errors stackResizeDown   (stack_t*  const stack_p);



// int main(){
//
//     stack_t* stack_p = NULL;
//
//     int error_code = correct;
//
//     if((error_code = createAndInitStack(&stack_p, DEFAULT_STACK_SIZE)) != correct){
//
//         printf(RED "Can't create a stack:\n" DEFAULT);
//         perror(RED "Error is fatal" ON_DEBUG(", \n Errno print:  " DEFAULT));
//         return errno;
//     }
//
//     ON_DEBUG(printStack(stack_p);)
//
//     char command[MAX_COMMAND_LEN] = "";
//     elem_type buffer = 0;
//
//     while(strcmp(command, "end") != 0){
//
//         scanf("%s", command);
//
//         if(strcmp(command, "push") == 0){
//
//             SCAN_ELEM(&buffer);
//             stackPush(stack_p, buffer);
//             ON_DEBUG(printStack(stack_p);)
//
//         }else if(strcmp(command, "pop") == 0){
//
//             stackPop(stack_p, &buffer);
//             printf("Last element: " VIOLET);
//             PRINT_ELEM(buffer);
//             printf("\n" DEFAULT);
//             ON_DEBUG(printStack(stack_p);)
//
//         }else if(strcmp(command, "break") == 0){
//
//             FIRST_ELEM_P(stack_p)[-1] = 33;
//             //FIRST_ELEM_P(stack_p)[3] = 33;
//             //stack_p->capacity = 3;
//             //stack_p->size = 3;
//             //stack_p = NULL;
//         }
//     }
//
//     stackDelete(stack_p);
// }

code_errors createAndInitStack(stack_t** const stack_p_p, const size_t capacity){

    MY_ASSERT(stack_p_p)
    MY_ASSERT(capacity > 0)

    *stack_p_p = (stack_t*) calloc(1, sizeof(stack_t));

    if(*stack_p_p == NULL){

        return memory_cannot_be_allocated_to_create_a_stack;
    }


    (*stack_p_p)->data = (char*) calloc(1 , capacity * sizeof(elem_type) + sizeof(TRUE_CANARY_DATA_LEFT) + sizeof(TRUE_CANARY_DATA_RIGHT));

    if((*stack_p_p)->data == NULL){

        free(*stack_p_p);

        return memory_cannot_be_allocated_to_create_a_stack;
    }

    stackInit((*stack_p_p), capacity);

    CHECK_STACK((*stack_p_p), "check on exit");

    return correct;
}

void stackInit(stack_t* const stack_p, const size_t capacity){

    MY_ASSERT(stack_p);


    ON_CANARY(stack_p->canary_left = TRUE_CANARY_STACK_LEFT;)
    stack_p->size = 0;
    stack_p->capacity = capacity;
    ON_CANARY(stack_p->canary_right = TRUE_CANARY_STACK_RIGHT;)

    ON_CANARY(CANARY_DATA_LEFT(stack_p)  = TRUE_CANARY_DATA_LEFT;)
    ON_CANARY(CANARY_DATA_RIGHT(stack_p) = TRUE_CANARY_DATA_RIGHT;)

    for(size_t i = 0; i < capacity; i++){

        #if IS_CANARY_PROTECT

            ((elem_type*) (stack_p->data + sizeof(TRUE_CANARY_DATA_LEFT)))[i] = POISON;

        #else

            ((elem_type*) stack_p->data)[i] = POISON;

        #endif
    }

    ON_HASH(recalculateHashes(stack_p);)

    CHECK_STACK(stack_p, "check on output");
}

code_errors        stackPush(stack_t* const stack_p, const elem_type new_elem){

    MY_ASSERT(stack_p)
    CHECK_STACK(stack_p, "check on input");

    code_errors error_code = correct;

    if(stack_p->size >= stack_p->capacity && (error_code = stackResizeUp(stack_p)) != correct){

        return error_code;
    }

    FIRST_ELEM_P(stack_p)[stack_p->size++] = new_elem;

    ON_HASH(recalculateHashes(stack_p);)

    CHECK_STACK(stack_p, "check on exit");

    return correct;
}

code_errors     stackPop(stack_t* const stack_p, elem_type* const last_elem_p){

    MY_ASSERT(stack_p)
    MY_ASSERT(last_elem_p)
    CHECK_STACK(stack_p, "check on input");

    if(stack_p->size != 0){

        *last_elem_p = FIRST_ELEM_P(stack_p)[stack_p->size - 1];
        FIRST_ELEM_P(stack_p)[stack_p->size - 1] = POISON;
        stack_p->size--;

        code_errors error_code = correct;

        ON_HASH(recalculateHashes(stack_p);)

        if(stack_p->size * 4 <= stack_p->capacity && stack_p->capacity != 1 && (error_code = stackResizeDown(stack_p)) != correct){

            return error_code;
        }

    }else{

        printf(ON_COMMON("Stack " CYAN "[%p]" DEFAULT " with " GREEN "%llu" DEFAULT " size and " YELLOW "%llu" DEFAULT " capacity error:\n" ) YELLOW
               "Stack underflow, there is no elements in stack\n" DEFAULT
               ON_COMMON(, stack_p, stack_p->size, stack_p->capacity));

        *last_elem_p = POISON;
    }

    ON_HASH(recalculateHashes(stack_p);)

    CHECK_STACK(stack_p, "check on exit");

    return correct;
}

void        stackDelete(stack_t* const stack_p){

    MY_ASSERT(stack_p)
    CHECK_STACK(stack_p, "check on input");

    free(stack_p->data);
    free(stack_p);
}

code_errors stackResizeUp(stack_t* const stack_p){

    MY_ASSERT(stack_p)
    CHECK_STACK(stack_p, "check on input");

    void* new_pointer = realloc(stack_p->data,
    (stack_p->capacity * 2) * sizeof(elem_type) ON_CANARY( + sizeof(TRUE_CANARY_DATA_LEFT) + sizeof(TRUE_CANARY_DATA_RIGHT)));

    if(new_pointer == NULL){

        printf("Stack " CYAN ON_DEBUG("[%p]") DEFAULT " with " GREEN "%llu" DEFAULT " size and " YELLOW "%llu" DEFAULT " capacity error:\n" YELLOW
               "Stack overflow, memory cannot be allocated for stack expansion\n"
                ON_DEBUG("Errno output: %s"),
                ON_DEBUG(stack_p,) stack_p->size, stack_p->capacity ON_DEBUG(, strerror(errno)));

        CHECK_STACK(stack_p, "check on exit");

        return memory_cannot_be_allocated_for_stack_expansion;
    }

    stack_p->data = (char*) new_pointer;
    stack_p->capacity *= 2;
    ON_CANARY(CANARY_DATA_RIGHT(stack_p) = TRUE_CANARY_DATA_RIGHT;)

    for(size_t i = stack_p->capacity / 2; i < stack_p->capacity; i++){

        FIRST_ELEM_P(stack_p)[i] = POISON;
    }

    ON_DEBUG(printf("Stack " VIOLET "reallocate" DEFAULT ", new stack capacity: " YELLOW "%llu\n" DEFAULT, stack_p->capacity);)

    ON_HASH(recalculateHashes(stack_p);)

    CHECK_STACK(stack_p, "check on exit");

    return correct;
}

code_errors stackResizeDown(stack_t* const stack_p){

    MY_ASSERT(stack_p)
    CHECK_STACK(stack_p, "check on input");


    void* new_pointer = realloc(stack_p->data,
    (stack_p->capacity / 2) * sizeof(elem_type) ON_CANARY( + sizeof(TRUE_CANARY_DATA_LEFT) + sizeof(TRUE_CANARY_DATA_RIGHT)));

    if(new_pointer == NULL){

        printf("Stack " CYAN ON_DEBUG("[%p]") DEFAULT " with " GREEN "%llu" DEFAULT " size and " YELLOW "%llu" DEFAULT " capacity error:\n" YELLOW
               "Memory cannot be allocated for stack reducing\n"
                ON_DEBUG("Errno output: %s"),
                ON_DEBUG(stack_p,) stack_p->size, stack_p->capacity ON_DEBUG(, strerror(errno)));

        CHECK_STACK(stack_p, "check on exit");

        return memory_cannot_be_allocated_for_stack_expansion;
    }

    stack_p->data = (char*) new_pointer;
    stack_p->capacity /= 2;
    ON_CANARY(CANARY_DATA_RIGHT(stack_p) = TRUE_CANARY_DATA_RIGHT;)

    ON_DEBUG(printf("Stack " VIOLET "reallocate" DEFAULT ", new stack capacity: " YELLOW "%llu\n" DEFAULT, stack_p->capacity);)

    ON_HASH(recalculateHashes(stack_p);)

    CHECK_STACK(stack_p, "check on exit");

    return correct;
}
