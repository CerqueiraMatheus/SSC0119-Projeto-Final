#ifndef ULA_H
#define ULA_H


#include <stdbool.h>

#include "operacao.h"


typedef struct ResultadoUla {
	unsigned int valor;
	unsigned int fr;
} resultado_ula_t;


resultado_ula_t ula(unsigned int x, unsigned int y, int carry, bool tem_carry, operacao_t operacao);
unsigned int rotaciona_esquerda(unsigned int valor, int shift);
unsigned int rotaciona_direita(unsigned int valor, int shift);


#endif // ULA_H
