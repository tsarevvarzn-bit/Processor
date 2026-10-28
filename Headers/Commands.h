#ifndef COMMANDS_H
#define COMMANDS_H

enum commands_codes{//Пять правый битов

    ZERO  = 0x00,
    PUSH  = 0x01,
    POP   = 0x02,
    JUMP  = 0x03,
    CALL  = 0x04,
    RET   = 0x05,
    IN    = 0x06,
    OUT   = 0x07,
    DRAW  = 0x08,
    DELAY = 0x09,
    HLT   = 0x0A,

    ADD   = 0x0B,
    SUB   = 0x0C,
    MUL   = 0x0D,
    DIV   = 0x0E,
    SQRT  = 0x0F,
    RND   = 0x10,
};

enum jump_arguments{//Три левых бита

    JUMP_ARG_ABS = 0b0000'0000,
    JUMP_ARG_A   = 0b0010'0000,
    JUMP_ARG_AE  = 0b0100'0000,
    JUMP_ARG_B   = 0b0110'0000,
    JUMP_ARG_BE  = 0b1000'0000,
    JUMP_ARG_E   = 0b1010'0000,
    JUMP_ARG_NE  = 0b1100'0000,
};

enum push_arguments{

    PUSH_ARG_IMMED_CONST = 0b0010'0000,
    PUSH_ARG_REG         = 0b0100'0000,
    PUSH_ARG_RAM         = 0b1000'0000,
};

enum pop_arguments{

    POP_ARG_IMMED_CONST = 0b0010'0000,
    POP_ARG_REG         = 0b0100'0000,
    POP_ARG_RAM         = 0b1000'0000,
};

enum registers_codes{

    AX =  0x01,
    BX =  0x02,
    CX =  0x03,
    DX =  0x04,
};

#endif
