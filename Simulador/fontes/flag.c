#include <stdbool.h>
#include <math.h>

#include "operacao.h"


unsigned int fr_para_inteiro(bool fr[]) {
    unsigned int inteiro = 0;

    for (int i = TAMANHO_PALAVRA - 1; i >= 0; i--) {
        inteiro += (unsigned int)fr[i] * pow(2, i);
    }

    return inteiro;
}
