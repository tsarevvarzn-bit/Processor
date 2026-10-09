void        checkStack     (const stack_t*  const stack_p,
                            const char* const     file_name,
                            unsigned int          line,
                            const char* const     func_name,
                            const char* const     comment);

code_errors verifyStack    (const stack_t*  const stack_p, const char* const call_info);
code_errors verifyCommon   (const stack_t* const stack_p, const char* const call_info);
code_errors verifyCanaries (const stack_t*  const stack_p, const char* const call_info);

ON_HASH(

code_errors verifyHash     (const stack_t*  const stack_p, const char* const call_info);

void   recalculateHashes(stack_t* const stack_p);
size_t calculateHash(const void* const begining_p, const size_t len);

)

ON_DEBUG(

void checkStack(const stack_t* const stack_p, const char* const file_name, unsigned int line, const char* const func_name, const char* const comment){

    MY_ASSERT(file_name)
    MY_ASSERT(func_name)
    MY_ASSERT(comment)

    char str[100] = "";
    sprintf(str, "\nCheck of stack was called in %s:%d in function %s, comment: %s", file_name, line, func_name, comment);

    if(verifyStack(stack_p, str) != correct){

        abort();
    }
}

code_errors verifyStack(const stack_t* const stack_p, const char* const call_info){ //TODO тихий\громкий режим

    MY_ASSERT(stack_p)
    MY_ASSERT(call_info)

    code_errors err_code = correct;

    ON_COMMON(

        err_code =  verifyCommon(stack_p, call_info);

        if(err_code != correct)
            return err_code;
    )

    ON_CANARY(

        err_code =  verifyCanaries(stack_p, call_info);

        if(err_code != correct)
            return err_code;
    )

    ON_HASH(

        err_code =  verifyHash(stack_p, call_info);

        if(err_code != correct)
            return err_code;
    )

    return err_code;
}

)

ON_COMMON(

code_errors verifyCommon(const stack_t* const stack_p, const char* const call_info){

    MY_ASSERT(stack_p)
    MY_ASSERT(call_info)

    if(stack_p->data == NULL){

        printf(ON_DEBUG(VIOLET "%s\n") DEFAULT "Stack" ON_DEBUG(CYAN "[%p]") DEFAULT " with " GREEN "%llu" DEFAULT " size and " YELLOW "%llu" DEFAULT " capacity is incorrect:\n" RED
               ON_DEBUG("stack_p->data == NULL" YELLOW ", stack dump:\n") DEFAULT,
               ON_DEBUG(call_info, stack_p,) stack_p->size, stack_p->capacity);

        printStack(stack_p);

        return stack_data_is_NULL;
    }

    if(stack_p->capacity == 0){

        printf(ON_DEBUG(VIOLET "%s\n") DEFAULT "Stack" ON_DEBUG(CYAN "[%p]") DEFAULT " with " GREEN "%llu" DEFAULT " size and " YELLOW "%llu" DEFAULT " capacity is incorrect:\n" RED
               ON_DEBUG("capacity = 0" YELLOW ", stack dump:\n") DEFAULT,
               ON_DEBUG(call_info, stack_p,) stack_p->size, stack_p->capacity);

        printStack(stack_p);

        return stack_capacity_is_zero;
    }

    if(stack_p->size > stack_p->capacity){

        printf(ON_DEBUG(VIOLET "%s\n") DEFAULT "Stack" ON_DEBUG(CYAN "[%p]") DEFAULT " with " GREEN "%llu" DEFAULT " size and " YELLOW "%llu" DEFAULT " capacity is incorrect:\n" RED
               ON_DEBUG("size > capacity" YELLOW ", stack dump:\n") DEFAULT,
               ON_DEBUG(call_info, stack_p,) stack_p->size, stack_p->capacity);

        printStack(stack_p);

        return stack_size_larger_than_capacity;
    }

    return correct;

}

)

ON_CANARY(

code_errors verifyCanaries(const stack_t* const stack_p, const char* const call_info){

    MY_ASSERT(stack_p)
    MY_ASSERT(call_info)

    //TODO ON_DEBUG ничего не меняет

    if(stack_p->canary_left != TRUE_CANARY_STACK_LEFT){

        printf(ON_DEBUG( VIOLET "%s\n" ) DEFAULT "Stack" ON_DEBUG( CYAN "[%p]" ) DEFAULT " with " GREEN "%llu" DEFAULT " size and " YELLOW "%llu" DEFAULT " capacity is incorrect" ON_DEBUG( ":\n" RED
               "left canary in stack is dead: " YELLOW "true value of canary: %llX, real value: %llX, stack dump:\n" DEFAULT),
               ON_DEBUG(call_info, stack_p,) stack_p->size, stack_p->capacity ON_DEBUG(, TRUE_CANARY_STACK_LEFT, stack_p->canary_left));

        printStack(stack_p);

        return left_canary_in_stack_is_dead;

    }

    if(stack_p->canary_right != TRUE_CANARY_STACK_RIGHT){

        printf(ON_DEBUG( VIOLET "%s\n" ) DEFAULT "Stack" ON_DEBUG( CYAN "[%p]" ) DEFAULT " with " GREEN "%llu" DEFAULT " size and " YELLOW "%llu" DEFAULT " capacity is incorrect" ON_DEBUG( ":\n" RED
            "right canary in stack is dead: " YELLOW "true value of canary: %llX, real value: %llX, stack dump:\n" DEFAULT),
            ON_DEBUG(call_info, stack_p,) stack_p->size, stack_p->capacity ON_DEBUG(, TRUE_CANARY_STACK_RIGHT, stack_p->canary_right));

        printStack(stack_p);

        return right_canary_in_stack_is_dead;

    }

    if(CANARY_DATA_LEFT(stack_p) != TRUE_CANARY_DATA_LEFT){

        printf(ON_DEBUG( VIOLET "%s\n" ) DEFAULT "Stack" ON_DEBUG( CYAN "[%p]" ) DEFAULT " with " GREEN "%llu" DEFAULT " size and " YELLOW "%llu" DEFAULT " capacity is incorrect" ON_DEBUG( ":\n" RED
            "left canary in data is dead: " YELLOW "true value of canary: %llX, real value: %llX, stack dump:\n" DEFAULT),
            ON_DEBUG(call_info, stack_p,) stack_p->size, stack_p->capacity ON_DEBUG(, TRUE_CANARY_DATA_LEFT, CANARY_DATA_LEFT(stack_p)));

        printStack(stack_p);

        return left_canary_in_data_is_dead;

    }

    if(CANARY_DATA_RIGHT(stack_p)  != TRUE_CANARY_DATA_RIGHT){

        printf(ON_DEBUG( VIOLET "%s\n" ) DEFAULT "Stack" ON_DEBUG( CYAN "[%p]" ) DEFAULT " with " GREEN "%llu" DEFAULT " size and " YELLOW "%llu" DEFAULT " capacity is incorrect" ON_DEBUG( ":\n" RED
            "right canary in data is dead: " YELLOW "true value of canary: %llX, real value: %llX, stack dump:\n" DEFAULT),
            ON_DEBUG(call_info, stack_p,) stack_p->size, stack_p->capacity ON_DEBUG(, TRUE_CANARY_DATA_RIGHT, CANARY_DATA_RIGHT(stack_p)));

        printStack(stack_p);

        return right_canary_in_data_is_dead;

    }

    return correct;
}
)

ON_HASH(

code_errors verifyHash(const stack_t* const stack_p, const char* const call_info){

    MY_ASSERT(stack_p)
    MY_ASSERT(call_info)

    size_t real_data_hash   = calculateHash(stack_p->data, stack_p->capacity * sizeof(elem_type) ON_CANARY(+ sizeof(TRUE_CANARY_DATA_LEFT) + sizeof(TRUE_CANARY_DATA_RIGHT)));
    size_t real_struct_hash = calculateHash(stack_p, ((const char*) (&(stack_p->data_hash))) - ((const char*) stack_p) + sizeof(stack_p->data_hash));

    if(real_data_hash != stack_p->data_hash){

        printf(ON_DEBUG( VIOLET "%s\n" ) DEFAULT "Stack" ON_DEBUG( CYAN "[%p]" ) DEFAULT " with " GREEN "%llu" DEFAULT " size and " YELLOW "%llu" DEFAULT " capacity is incorrect" ON_DEBUG( ":\n" RED
            "data hash is incorrect: " YELLOW "true value of hash: %llX, real value: %llX, stack dump:\n" DEFAULT),
            ON_DEBUG(call_info, stack_p,) stack_p->size, stack_p->capacity ON_DEBUG(, real_data_hash, stack_p->data_hash));

        printStack(stack_p);

        return data_hash_is_incorrect;

    }

    if(real_struct_hash != stack_p->struct_hash){

        printf(ON_DEBUG( VIOLET "%s\n" ) DEFAULT "Stack" ON_DEBUG( CYAN "[%p]" ) DEFAULT " with " GREEN "%llu" DEFAULT " size and " YELLOW "%llu" DEFAULT " capacity is incorrect" ON_DEBUG( ":\n" RED
            "struct hash is incorrect: " YELLOW "true value of hash: %llX, real value: %llX, stack dump:\n" DEFAULT),
            ON_DEBUG(call_info, stack_p,) stack_p->size, stack_p->capacity ON_DEBUG(, real_struct_hash, stack_p->struct_hash));

        printStack(stack_p);

        return struct_hash_is_incorrect;
    }

    return correct;
}

void recalculateHashes(stack_t* const stack_p){

    MY_ASSERT(stack_p)

    stack_p->data_hash   = calculateHash(stack_p->data, stack_p->capacity * sizeof(elem_type) ON_CANARY(+ sizeof(TRUE_CANARY_DATA_LEFT) + sizeof(TRUE_CANARY_DATA_RIGHT)));
    stack_p->struct_hash = calculateHash(stack_p, ((char*) (&(stack_p->data_hash))) - ((char*) stack_p) + sizeof(stack_p->data_hash));

}

size_t calculateHash(const void* const begining_p, const size_t len){

    MY_ASSERT(begining_p)

    const char* begining_char_p = (const char*) begining_p;

    size_t hash = 5381;

    for(size_t i = 0; i < len; i++){

        hash = hash * 33 + begining_char_p[i];
    }

    return hash;
}

)
//TODO calculateHASHsForStack
