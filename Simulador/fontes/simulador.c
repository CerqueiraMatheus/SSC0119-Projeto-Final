#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "flag.h"
#include "memoria.h"
#include "menu.h"
#include "operacao.h"
#include "teclado.h"
#include "ula.h"

int main() {
    unsigned int memoria[TAMANHO_MEMORIA];
    le_arquivo_memoria(memoria);

    cpu_t cpu = cria_cpu();

    operacao_t operacao;
    operacao_t operacao_ula;

    bool tem_carry = false;
    resultado_ula_t resultado_ula;

    acesso_t rw = LEITURA;
    unsigned int valor_memoria = 0;

    char tecla;
    char opcao;

    imprime_cabecalho();

    do {
        atualiza_registradores(&cpu, valor_memoria);

        unsigned int rx = pega_pedaco(cpu.ir.valor, 9, 7);
        unsigned int ry = pega_pedaco(cpu.ir.valor, 6, 4);
        unsigned int rz = pega_pedaco(cpu.ir.valor, 3, 1);

        if (rw == ESCRITA) {
            memoria[cpu.mux[M1].valor] = cpu.mux[M5].valor;
        }
        rw = LEITURA;

        reseta_sinais_registradores(&cpu);
        cpu.mux[M4].selecao = 0;

        switch (cpu.estado) {
            case RESETA:
                cpu = cria_cpu();
                tem_carry = false;
                rw = LEITURA;
                valor_memoria = 0;
                break;

            case BUSCA:
                cpu.mux[M1].selecao = PC;
                rw = LEITURA;
                cpu.ir.load = true;
                cpu.pc.incrementa = true;

                cpu.estado = DECODIFICA;
                break;

            case DECODIFICA:
                operacao = pega_pedaco(cpu.ir.valor, 15, 10);

                switch (operacao) {
                    case INCHAR:
                        if (esta_teclando()) {
                            tecla = getchar();
                        } else {
                            tecla = TECLA_PADRAO;
                        }

                        tecla = pega_pedaco(tecla, 7, 0);
                        cpu.mux[M2].selecao = TECLADO;
                        cpu.registradores[rx].load = true;

                        cpu.estado = BUSCA;
                        break;

                    case OUTCHAR:
                        putchar(cpu.registradores[rx].valor);

                        cpu.estado = BUSCA;
                        break;

                    case LOADN:
                        cpu.mux[M1].selecao = PC;
                        rw = LEITURA;
                        cpu.mux[M2].selecao = MEMORIA;
                        cpu.registradores[rx].load = true;
                        cpu.pc.incrementa = true;

                        cpu.estado = BUSCA;
                        break;

                    case LOAD:
                        cpu.mux[M1].selecao = PC;
                        rw = LEITURA;
                        cpu.mar.load = true;
                        cpu.pc.incrementa = true;

                        cpu.estado = EXECUTA_1;
                        break;

                    case LOADI:
                        cpu.mux[M4].selecao = ry;
                        cpu.mux[M1].selecao = MUX_4;
                        rw = LEITURA;
                        cpu.mux[M2].selecao = MEMORIA;
                        cpu.registradores[rx].load = true;

                        cpu.estado = BUSCA;
                        break;

                    case STORE:
                        cpu.mux[M1].selecao = PC;
                        rw = LEITURA;
                        cpu.mar.load = true;
                        cpu.pc.incrementa = true;

                        cpu.estado = EXECUTA_1;
                        break;

                    case STOREI:
                        cpu.mux[M4].selecao = rx;
                        cpu.mux[M1].selecao = MUX_4;
                        rw = ESCRITA;
                        cpu.mux[M3].selecao = ry;
                        cpu.mux[M5].selecao = MUX_3;

                        cpu.estado = BUSCA;
                        break;

                    case MOV:;
                        unsigned int val = pega_pedaco(cpu.ir.valor, 0, 0);

                        if (val == 0) {
                            cpu.mux[M4].selecao = ry;
                            cpu.mux[M2].selecao = MUX_4;
                            cpu.registradores[rx].load = true;
                        }

                        else if (val == 1) {
                            cpu.mux[M2].selecao = SP;
                            cpu.registradores[rx].load = true;
                        }

                        else {
                            cpu.mux[M4].selecao = rx;
                            cpu.sp.load = true;
                        }

                        cpu.estado = BUSCA;
                        break;

                    case AND ... NOT:
                    case ADD ... DIV:
                    case MOD:
                        cpu.mux[M3].selecao = ry;
                        cpu.mux[M4].selecao = rz;
                        operacao_ula = operacao;
                        cpu.mux[M2].selecao = ULA;
                        cpu.registradores[rx].load = true;
                        tem_carry = pega_pedaco(cpu.ir.valor, 0, 0);

                        cpu.estado = BUSCA;
                        break;

                    case ADDN:
                        cpu.mux[M1].selecao = PC;
                        rw = LEITURA;

                        cpu.mux[M3].selecao = ry;
                        cpu.mux[M4].selecao = IMEDIATO;
                        cpu.mux[M2].selecao = ULA;
                        cpu.registradores[rx].load = true;
                        tem_carry = pega_pedaco(cpu.ir.valor, 0, 0);
                        operacao_ula = ADD;

                        cpu.pc.incrementa = true;
                        cpu.estado = BUSCA;
                        break;

                    case INC:
                        cpu.mux[M3].selecao = rx;
                        cpu.mux[M4].selecao = UM;

                        if (pega_pedaco(cpu.ir.valor, 6, 6) == 0) {
                            operacao_ula = ADD;
                        }

                        else {
                            operacao_ula = SUB;
                        }

                        cpu.mux[M2].selecao = ULA;
                        cpu.registradores[rx].load = true;

                        cpu.estado = BUSCA;
                        break;

                    case CMP:
                        cpu.mux[M3].selecao = rx;
                        cpu.mux[M4].selecao = ry;
                        operacao_ula = CMP;
                        cpu.mux[M6].selecao = ULA;
                        cpu.fr.load = true;

                        cpu.estado = BUSCA;
                        break;

                    case SHIFT:
                        cpu.registradores[rx].valor = shift(cpu.registradores[rx].valor, cpu.ir.valor);
                        cpu.fr.flags[ZERO] = cpu.registradores[rx].valor == 0;

                        cpu.estado = BUSCA;
                        break;

                    case JMP:;
                        jump_t jump = pega_pedaco(cpu.ir.valor, 9, 6);

                        if (
                            jump == JUMP ||
                            (jump == JEQ && cpu.fr.flags[IGUAL]) ||
                            (jump == JNE && !cpu.fr.flags[IGUAL]) ||
                            (jump == JZ && cpu.fr.flags[ZERO]) ||
                            (jump == JNZ && !cpu.fr.flags[ZERO]) ||
                            (jump == JC && cpu.fr.flags[CARRY]) ||
                            (jump == JNC && !cpu.fr.flags[CARRY]) ||
                            (jump == JGR && cpu.fr.flags[MAIOR]) ||
                            (jump == JLE && cpu.fr.flags[MENOR]) ||
                            (jump == JEG && (cpu.fr.flags[IGUAL] || cpu.fr.flags[MAIOR])) ||
                            (jump == JEL && (cpu.fr.flags[IGUAL] || cpu.fr.flags[MENOR])) ||
                            (jump == JOV && cpu.fr.flags[OVERFLOW_ARITMETICO]) ||
                            (jump == JNOV && !cpu.fr.flags[OVERFLOW_ARITMETICO]) ||
                            (jump == JN && cpu.fr.flags[NEGATIVO]) ||
                            (jump == JDZ && cpu.fr.flags[DIVISAO_POR_ZERO])) {
                            cpu.mux[M1].selecao = PC;
                            rw = LEITURA;
                            cpu.pc.load = true;
                        } else {
                            cpu.pc.incrementa = true;
                        }

                        cpu.estado = BUSCA;
                        break;

                    case CALL:

                        cpu.estado = BUSCA;
                        break;

                    case PUSH:

                        cpu.estado = BUSCA;
                        break;

                    case POP:

                        cpu.estado = EXECUTA_1;
                        break;

                    case RTS:

                        cpu.estado = EXECUTA_1;
                        break;

                    case SETC:
                        cpu.fr.flags[CARRY] = pega_pedaco(cpu.ir.valor, 9, 9);

                        cpu.estado = BUSCA;
                        break;

                    case HALT:
                        cpu.estado = ESPERA;
                        break;

                    case NOP:
                        cpu.estado = BUSCA;
                        break;

                    case BREAKP:
                        getchar();

                        cpu.estado = BUSCA;
                        break;

                    default:
                        cpu.estado = BUSCA;
                        break;
                }
                break;

            case EXECUTA_1:
                switch (operacao) {
                    case LOAD:
                        cpu.mux[M1].selecao = MAR;
                        rw = LEITURA;
                        cpu.mux[M2].selecao = MEMORIA;
                        cpu.registradores[rx].load = true;

                        cpu.estado = BUSCA;
                        break;

                    case STORE:
                        cpu.mux[M1].selecao = MAR;
                        rw = ESCRITA;
                        cpu.mux[M3].selecao = rx;
                        cpu.mux[M5].selecao = MUX_3;

                        cpu.estado = BUSCA;
                        break;

                    case CALL:
                        cpu.mux[M1].selecao = PC;
                        rw = LEITURA;
                        cpu.pc.load = true;

                        cpu.estado = BUSCA;
                        break;

                    case ADDN:
                        cpu.pc.incrementa = true;
                        cpu.estado = BUSCA;
                        break;

                    case POP:

                        cpu.estado = BUSCA;
                        break;

                    case RTS:

                        cpu.estado = EXECUTA_2;
                        break;

                    case PUSH:

                        cpu.estado = BUSCA;
                        break;
                }
                break;

            case EXECUTA_2:
                cpu.estado = BUSCA;
                break;

            case ESPERA:
                putchar('\n');
                putchar('\n');
                imprime_opcoes();
                opcao = le_opcao();

                cpu.estado = RESETA;
                break;
        }

        atualiza_mux_4(&cpu);
        atualiza_mux_1(&cpu);
        atualiza_mux_3(&cpu);

        if (rw == LEITURA) {
            valor_memoria = memoria[cpu.mux[M1].valor];
        }

        if (cpu.mux[M4].selecao == IMEDIATO) {
            cpu.mux[M4].valor = valor_memoria;
        }

        resultado_ula = ula(cpu.mux[M3].valor, cpu.mux[M4].valor,
                            cpu.fr.flags[CARRY], tem_carry, operacao_ula);

        atualiza_mux_2(&cpu, resultado_ula.valor, valor_memoria, tecla);
        atualiza_mux_5(&cpu);
        atualiza_mux_6(&cpu, resultado_ula.fr, valor_memoria);

    } while (opcao != SAIR);

    return EXIT_SUCCESS;
}
