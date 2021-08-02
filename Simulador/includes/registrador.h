#ifndef REGISTRADOR_H
#define REGISTRADOR_H

#include <stdbool.h>

#define NUMERO_REGISTRADORES 8

typedef struct Registrador {
    unsigned int valor;
    bool incrementa;
    bool decrementa;
    bool load;
}
registrador_t;

registrador_t cria_registrador();
void cria_registradores(registrador_t registradores[]);

#endif // REGISTRADOR_H
