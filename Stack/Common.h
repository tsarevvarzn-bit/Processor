#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#define DEFAULT "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define VIOLET  "\033[1;35m"
#define CYAN    "\033[1;36m"

const unsigned int MAX_COMMAND_LEN    = 100;


enum code_errors{

    correct,

    stack_data_is_NULL,             //Фатальные ошибки программиста
    stack_size_larger_than_capacity,
    stack_capacity_is_zero,

    left_canary_in_stack_is_dead,
    right_canary_in_stack_is_dead,
    left_canary_in_data_is_dead,
    right_canary_in_data_is_dead,

    data_hash_is_incorrect,
    struct_hash_is_incorrect,

    memory_cannot_be_allocated_for_stack_expansion, //Не фатальные, должна быть обработана + взаимодействие с пользователем
    memory_cannot_be_allocated_for_stack_reducing,
    memory_cannot_be_allocated_to_create_a_stack    //Фатально для конкретного стэка
};
