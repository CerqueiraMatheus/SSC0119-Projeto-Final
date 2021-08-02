#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "operacao.h"
#include "memoria.h"
#include "teclado.h"
#include "flag.h"
#include "cpu.h"
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

inicio:
	printf("PROCESSADOR ICMC  - Menu:\n");
	printf("                          'r' goto inicio...\n");
	printf("                          'q' goto fim...\n\n");
	printf("Rodando...\n");

	cpu.estado = RESETA;

loop:
	atualiza_registradores(&cpu, valor_memoria);

	int rx = pega_pedaco(cpu.ir.valor, 9, 7);
	int ry = pega_pedaco(cpu.ir.valor, 6, 4);
	int rz = pega_pedaco(cpu.ir.valor, 3, 1);

	if (rw == ESCRITA) {
		memoria[cpu.mux[M1].valor] = cpu.mux[M5].valor;
	}
	rw = LEITURA;

	reseta_sinais_registradores(&cpu);

	switch(cpu.estado) {
		case RESETA:
			cpu = cria_cpu();
			rw = LEITURA;
			valor_memoria = 0;

			cpu.estado = BUSCA;
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

			switch(operacao){
				case INCHAR:
					if(esta_teclando()) {
						tecla = getchar();
					}
					else {
						tecla = TECLA_PADRAO;
					}

					tecla = pega_pedaco(tecla,7,0);
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
					
					cpu.estado = BUSCA;
					break;

				case STORE:

					cpu.estado = EXECUTA_1;
					break;

				case STOREI:

					cpu.estado = BUSCA;
					break;

				case MOV:
					
					cpu.estado = BUSCA;
					break;

				case AND ... NOT:
				case ADD ... DIV:
				case MOD:

					cpu.estado = BUSCA;
					break;

				case INC:

					cpu.estado = BUSCA;
					break;

				case CMP:

					cpu.estado = BUSCA;
					break;

				case SHIFT:
					switch(pega_pedaco(cpu.ir.valor, 6, 4)) {
						case 0: 
							cpu.registradores[rx].valor = cpu.registradores[rx].valor << pega_pedaco(cpu.ir.valor, 3, 0);
							break;
						
						case 1:
							cpu.registradores[rx].valor = ~(~(cpu.registradores[rx].valor) << pega_pedaco(cpu.ir.valor, 3, 0));
							break;
						
						case 2:
							cpu.registradores[rx].valor = cpu.registradores[rx].valor >> pega_pedaco(cpu.ir.valor, 3, 0);
							break;
						
						case 3:
							cpu.registradores[rx].valor = ~(~(cpu.registradores[rx].valor) >> pega_pedaco(cpu.ir.valor,3,0));
							break;
						
						default:
								if(pega_pedaco(cpu.ir.valor, 6, 5) == 2) { // ROTATE LEFT
									cpu.registradores[rx].valor = rotaciona_esquerda(cpu.registradores[rx].valor, pega_pedaco(cpu.ir.valor, 3, 0));
								}
								else {
									cpu.registradores[rx].valor = rotaciona_direita(cpu.registradores[rx].valor, pega_pedaco(cpu.ir.valor, 3, 0)); 
								}
								break;
					}

					cpu.fr.flags[ZERO] = cpu.registradores[rx].valor == 0;

					cpu.estado = BUSCA;
					break;

				case JMP: ;
					jump_t jump = pega_pedaco(cpu.ir.valor, 9, 6);

					if (
						jump == JUMP
						|| (jump == JEQ  &&  cpu.fr.flags[IGUAL])
						|| (jump == JNE  && !cpu.fr.flags[IGUAL])
						|| (jump == JZ   &&  cpu.fr.flags[ZERO])
						|| (jump == JNZ  && !cpu.fr.flags[ZERO])
						|| (jump == JC   &&  cpu.fr.flags[CARRY])
						|| (jump == JNC  && !cpu.fr.flags[CARRY])
						|| (jump == JGR  &&  cpu.fr.flags[MAIOR])
						|| (jump == JLE  &&  cpu.fr.flags[MENOR])
						|| (jump == JEG  && (cpu.fr.flags[IGUAL] || cpu.fr.flags[MAIOR]))
						|| (jump == JEL  && (cpu.fr.flags[IGUAL] || cpu.fr.flags[MENOR]))
						|| (jump == JOV  &&  cpu.fr.flags[OVERFLOW_ARITMETICO])
						|| (jump == JNOV && !cpu.fr.flags[OVERFLOW_ARITMETICO])
						|| (jump == JN 	 &&  cpu.fr.flags[NEGATIVO])
						|| (jump == JDZ  &&  cpu.fr.flags[DIVISAO_POR_ZERO])
					) {
						cpu.mux[M1].selecao = PC;
						rw = LEITURA;
						cpu.pc.load = true;
					}
					else {
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
			switch(operacao) {
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

			//case RTS:
			//PC++;
			
			cpu.estado = BUSCA;
			break;

		case ESPERA:
			putchar('\n');
			char key = getchar();
			if (key == 'r') goto inicio;
			if (key == 'q') goto fim;
			break;

		default:
			break;
	}

	atualiza_mux_4(&cpu);
	atualiza_mux_1(&cpu);
	atualiza_mux_3(&cpu);

	if (rw == LEITURA) {
		valor_memoria = memoria[cpu.mux[M1].valor];
	}

	resultado_ula = ula(cpu.mux[M3].valor, cpu.mux[M4].valor, cpu.fr.flags[CARRY], tem_carry, operacao_ula);

	atualiza_mux_2(&cpu, resultado_ula.valor, valor_memoria, tecla);
	atualiza_mux_5(&cpu);
	atualiza_mux_6(&cpu, resultado_ula.fr, valor_memoria);

	goto loop;

fim:
	return 0;
}
