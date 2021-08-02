#ifndef ARQUIVO_H
#define ARQUIVO_H

#define TAMANHO_MEMORIA 32768 // 32 KB

enum LeituraEscrita {
    LEITURA,
    ESCRITA
};

void le_arquivo_memoria(unsigned int memoria[]);

#endif // ARQUIVO_H
