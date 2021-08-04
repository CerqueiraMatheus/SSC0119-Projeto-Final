#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "montador.h"
#include "parser.h"
#include "structs.h"

/* Versao do montador */
#define VERSAO "0.2"

/* Arquivos de entrada e saida */
FILE *in;
FILE *out;
char *OutputFileName;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Montador :\nUso : %s <arquivo texto de entrada> [<arquivo de saida>]\n", argv[0]);
#ifndef __linux__
        system("pause");
#endif
        exit(1);
    }
    int l;
    //processamento do input
    if (argc == 2) {
        //input so tem arquivo de entrada. O nome do arquivo de saida eh igual ao de entrada trocando a extensao .asm (se tiver) por .mif, senao adicionando a extensao .mif
        l = strlen(argv[1]);

        //string nao eh grande o suficiente para ter a extensao .asm
        if (l < 4) {
            OutputFileName = (char *)malloc((4 + l + 1) * sizeof(char));
            strcpy(OutputFileName, argv[1]);
            strcat(OutputFileName, ".mif");
            //coeficiente de cagaco, pra ter ctz q a string termina
            OutputFileName[l] = '\0';
        }
        //string tem a extensao .asm
        else if (argv[1][l - 1] == 'm' && argv[1][l - 2] == 's' && argv[1][l - 3] == 'a' && argv[1][l - 4] == '.') {
            OutputFileName = (char *)malloc((l + 1) * sizeof(char));
            strcpy(OutputFileName, argv[1]);
            OutputFileName[l] = '\0';
            //trocando a extensao
            OutputFileName[l - 1] = 'f';
            OutputFileName[l - 2] = 'i';
            OutputFileName[l - 3] = 'm';
        }
        //string eh grande, mas n tem a extensao
        else {
            OutputFileName = (char *)malloc((l + 4 + 1) * sizeof(char));
            strcpy(OutputFileName, argv[1]);
            OutputFileName[l] = '\0';
        }
    } else {
        //mudar o ponteiro eh o mais eficiente
        OutputFileName = argv[2];
    }

    printf("Montador v.%s\n", VERSAO);

    calcula_tamanho();

    CarregaPrograma(argv[1]);

    out = fopen(OutputFileName, "w");

    if (out == NULL) {
        parser_Abort("Impossivel criar arquivo para escrita!");
    }

    Montar();

    fclose(out);

    if (argc == 2) {
        free(OutputFileName);
    }

    return 0;
}
