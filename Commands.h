#ifndef COMMANDS_H
#define COMMANDS_H

enum commands_codes{

    PUSH  = '1',
    ADD   = '2',
    SUB   = '3',
    MUL   = '4',
    DIV   = '5',
    OUT   = '6',
    HLT   = '7',
    SQRT  = '8',
    PUSHr = '9',
    POPr  = '0' + 10,
};

enum registers_codes{

    AX = '1',
    BX = '2',
    CX = '3',
    DX = '4',
};

#endif
