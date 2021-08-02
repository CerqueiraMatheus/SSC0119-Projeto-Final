#ifndef CPU_H
#define CPU_H

typedef enum Estado {
    RESETA,
    BUSCA,
    DECODIFICA,
    EXECUTA_1,
    EXECUTA_2,
    ESPERA
}
estado_t;

#endif // CPU_H
