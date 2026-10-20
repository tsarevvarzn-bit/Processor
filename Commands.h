#ifndef COMMANDS_H
#define COMMANDS_H

enum commands_codes{//Пять правый битов

    PUSH  = 0x01,//Потом добавим пушить из оперативки
    POPr  = 0x02,//Потом добавим попить в оперативку
    JUMP  = 0x03,
    CALL  = 0x04,
    RET   = 0x05,
    IN    = 0x06,
    OUT   = 0x07,
    HLT   = 0x08,

    ADD   = 0x09,
    SUB   = 0x0A,
    MUL   = 0x0B,
    DIV   = 0x0C,
    SQRT  = 0x0D,
};

enum jump_arguments{//Три левых бита

    JUMP_ARG_ABS = 0b0000'0000,
    JUMP_ARG_A   = 0b0010'0000,
    JUMP_ARG_AE  = 0b0100'0000,
    JUMP_ARG_B   = 0b0110'0000,
    JUMP_ARG_BE  = 0b1000'0000,
    JUMP_ARG_E   = 0b1010'0000,
    JUMP_ARG_NE  = 0b1100'0000,//Декодировка времени
};

enum push_arguments{

    PUSH_ARG_IMMED_CONST = 0b0010'0000,
    PUSH_ARG_REG         = 0b0100'0000,
};

enum registers_codes{

    AX = '1',
    BX = '2',
    CX = '3',
    DX = '4',
};

#endif
