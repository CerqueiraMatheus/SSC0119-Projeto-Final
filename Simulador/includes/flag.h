#ifndef FLAG_H
#define FLAG_H


#include <stdbool.h>


enum Flag {
    MAIOR,
    MENOR,
    IGUAL,
    ZERO,
    CARRY,
    OVERFLOW_ARITMETICO,
    DIVISAO_POR_ZERO,
    OVERFLOW_PILHA,
    UNDERFLOW_PILHA,
    NEGATIVO
};


unsigned int fr_para_inteiro(bool fr[]);


#endif // FLAG_H
