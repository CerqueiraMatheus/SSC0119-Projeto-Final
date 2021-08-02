#include <stdbool.h>

#include "registrador.h"

registrador_t cria_registrador() {
    return (registrador_t) {.valor = 0, .incrementa = false, .decrementa = false, .load = false};
}

void cria_registradores(registrador_t registradores[]) {
    for (int i = 0; i < NUMERO_REGISTRADORES; i++) {
        registradores[i] = cria_registrador();
    }
}
