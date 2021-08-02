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

	int rx=0, ry=0, rz=0, COND=0, RW=0, DATA_OUT=0;

	int carry=0;// Flag do IR que indica se a ULA vai fazer operação com carry ou não 
	int opcode=0;
	int OP=0;  // ULA
	int tecla;
	resultado_ula_t resultadoUla;


inicio:
	printf("PROCESSADOR ICMC  - Menu:\n");
	printf("                          'r' goto inicio...\n");
	printf("                          'q' goto fim...\n\n");
	printf("Rodando...\n");

// Loop principal do processador: Nunca para!!
loop:
	atualiza_registradores(&cpu, DATA_OUT);

	// Carrega dados do Mux 2 para os registradores
	rx = pega_pedaco(cpu.ir.valor, 9, 7);
	ry = pega_pedaco(cpu.ir.valor, 6, 4);
	rz = pega_pedaco(cpu.ir.valor, 3, 1);

	// Operacao de Escrita da Memoria
	if (RW == ESCRITA) {
		memoria[cpu.mux[M1].valor] = cpu.mux[M5].valor;
	}
	RW = LEITURA;

	// Reinicializa os Loads dos registradores
	reseta_sinais_registradores(&cpu);

	// Maquina de Controle
	switch(cpu.estado)
	{
		case RESETA:
			cpu = cria_cpu();

			cpu.estado = BUSCA;
			break;

		case BUSCA:
			cpu.mux[M1].selecao = PC;
			RW = LEITURA;
			cpu.ir.load = true;
			cpu.pc.incrementa = true;

			cpu.estado = DECODIFICA;
			break;

		case DECODIFICA:
			opcode = pega_pedaco(cpu.ir.valor, 15, 10);

			switch(opcode){
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
					printf("%c", cpu.registradores[rx].valor);

					cpu.estado = BUSCA;
					break;

				case LOADN:
					cpu.mux[M1].selecao = PC;
					RW = LEITURA;
					cpu.mux[M2].selecao = MEMORIA;
					cpu.registradores[rx].load = true;
					cpu.pc.incrementa = true;

					cpu.estado = BUSCA;
					break;

				case LOAD:
					cpu.mux[M1].selecao = PC;
					RW = LEITURA;
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

				case ADD:
				case SUB:
				case MULT:
				case DIV:
				case MOD:
				case AND:
				case OR:
				case XOR:
				case NOT:

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
					cpu.fr.flags[3] = 0; // -- FR = <...|zero|equal|lesser|greater>
					if(cpu.registradores[rx].valor == 0) {
						cpu.fr.flags[3] = 1;  // Se resultado = 0, seta o Flag de Zero
					}

					cpu.estado = BUSCA;
					break;

				case JMP:
					COND = pega_pedaco(cpu.ir.valor, 9, 6);

					if((COND == 0)                       	                      // NO COND
							|| (cpu.fr.flags[0]==1 && (COND==7))                            // GREATER
							|| ((cpu.fr.flags[2]==1 || cpu.fr.flags[0]==1) && (COND==9))              // GREATER EQUAL
							|| (cpu.fr.flags[1]==1 && (COND==8))                            // LESSER
							|| ((cpu.fr.flags[2]==1 || cpu.fr.flags[1]==1) && (COND==10))             // LESSER EQUAL
							|| (cpu.fr.flags[2]==1 && (COND==1))                            // EQUAL
							|| (cpu.fr.flags[2]==0 && (COND==2))                            // NOT EQUAL
							|| (cpu.fr.flags[3]==1 && (COND==3))                            // ZERO
							|| (cpu.fr.flags[3]==0 && (COND==4))                            // NOT ZERO
							|| (cpu.fr.flags[4]==1 && (COND==5))                            // CARRY
							|| (cpu.fr.flags[4]==0 && (COND==6))                            // NOT CARRY
							|| (cpu.fr.flags[5]==1 && (COND==11))                           // OVERFLOW
							|| (cpu.fr.flags[5]==0 && (COND==12))                           // NOT OVERFLOW
							|| (cpu.fr.flags[6]==1 && (COND==14))                           // NEGATIVO
							|| (cpu.fr.flags[9]==1 && (COND==13)))                          // DIVBYZERO
					{
						cpu.mux[M1].selecao = PC;
						RW = LEITURA;
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
					cpu.fr.flags[4] = pega_pedaco(cpu.ir.valor, 9, 9);

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
			switch(opcode) {
				case LOAD:
					cpu.mux[M1].selecao = MAR;
					RW = LEITURA;
					cpu.mux[M2].selecao = MEMORIA;
					cpu.registradores[rx].load = true;

					cpu.estado = BUSCA;
					break;

				case STORE:
					cpu.mux[M1].selecao = MAR;
					RW = ESCRITA;
					cpu.mux[M3].selecao = rx;
					cpu.mux[M5].selecao = MUX_3;

					cpu.estado = BUSCA;
					break; 

				case CALL:
					cpu.mux[M1].selecao = PC;
					RW = LEITURA;
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
			printf("\n");
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

	// Operacao de Leitura da Memoria
	if (RW == LEITURA) {
		DATA_OUT = memoria[cpu.mux[M1].valor];
	}

	// Operacao da ULA
	resultadoUla = ula(cpu.mux[M3].valor, cpu.mux[M4].valor, cpu.fr.flags[CARRY], carry, OP);

	// Selecao do Mux2
	atualiza_mux_2(&cpu, resultadoUla.valor, DATA_OUT, tecla);
	atualiza_mux_5(&cpu);
	atualiza_mux_6(&cpu, resultadoUla.fr, DATA_OUT);

	goto loop;

fim:
	return 0;
}
