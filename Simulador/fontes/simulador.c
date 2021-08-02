#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "registrador.h"
#include "operacao.h"
#include "memoria.h"
#include "teclado.h"
#include "flag.h"
#include "cpu.h"
#include "ula.h"
#include "mux.h"



int main() {
	unsigned int memoria[TAMANHO_MEMORIA];
	le_arquivo_memoria(memoria);
	
	int i=0;
	int key=0;    // Le Teclado

	registrador_t registradores[NUMERO_REGISTRADORES];
	cria_registradores(registradores);

	registrador_t pc = cria_registrador();
	registrador_t ir = cria_registrador();
	registrador_t sp = cria_registrador();
	registrador_t mar = cria_registrador();

	int rx=0, ry=0, rz=0, COND=0, RW=0, DATA_OUT=0;

	registrador_flags_t fr = cria_registrador_flags();

	mux_t mux[NUMERO_MUX];
	for (int i = 0; i < NUMERO_MUX; i++) {
		mux[i].valor = 0;
		mux[i].selecao = 0;
	}

	int carry=0;// Flag do IR que indica se a ULA vai fazer operação com carry ou não 
	int opcode=0;
	estado_t estado = RESETA;
	int OP=0;  // ULA
	int tecla;
	resultado_ula_t resultadoUla;


inicio:
	printf("PROCESSADOR ICMC  - Menu:\n");
	printf("                          'r' goto inicio...\n");
	printf("                          'q' goto fim...\n\n");
	printf("Rodando...\n");

	estado = RESETA;

	// Loop principal do processador: Nunca para!!
loop:
	// Executa Load dos Registradores
	if(ir.load) ir.valor = DATA_OUT;

	if(pc.load) pc.valor = DATA_OUT;

	if(pc.incrementa) pc.valor++;

	if(mar.load) mar.valor = DATA_OUT;

	if(sp.load) sp.valor = mux[M4].valor;

	if(sp.incrementa) sp.valor++;

	if(sp.decrementa) sp.valor--;

	if(fr.load) {
		inteiro_para_flags(mux[M6].valor, fr.flags);
	}

	// Carrega dados do Mux 2 para os registradores
	rx = pega_pedaco(ir.valor, 9, 7);
	ry = pega_pedaco(ir.valor, 6, 4);
	rz = pega_pedaco(ir.valor, 3, 1);
	
	// Coloca valor do Mux2 para o registrador com Load
	if(registradores[rx].load) registradores[rx].valor = mux[M2].valor;

	// Operacao de Escrita da Memoria
	if (RW == 1) memoria[mux[M1].valor] = mux[M5].valor;

	// ---------------------------------------

	// Reinicializa os Loads dos registradores
	for(i = 0; i < NUMERO_REGISTRADORES; i++) {
		registradores[i].load = false;
	}

	RW = 0;

	ir.load  = false;
	mar.load = false;
	pc.load  = false;
	pc.incrementa   = false;
	sp.load  = false;
	sp.incrementa   = false;
	sp.decrementa   = false;
	fr.load  = false;


	// Maquina de Controle
	switch(estado)
	{
		case RESETA:
			// Reinicializa os registradores
			for(i=0;i<8;i++) {
				registradores[i].valor = 0;
				registradores[i].load = false;
			}
			for(i=0;i<16;i++) {
				fr.flags[i] = false;
			}

			pc.valor = 0;  // inicializa na linha Zero da memoria -> Programa tem que comecar na linha Zero !!
			ir.valor = 0;
			mar.valor = 0;
			sp.valor = TAMANHO_MEMORIA -1;

			RW = 0;
			DATA_OUT = 0;  // Barramento de saida de Dados da Memoria

			ir.load  = false;
			mar.load = false;
			pc.load  = false;
			pc.incrementa   = false;
			sp.load  = false;
			sp.incrementa   = false;
			sp.decrementa   = false;
			fr.load  = false;
			mux[M1].selecao   = PC;
			mux[M2].selecao   = DADO_SAIDA;
			mux[M3].selecao   = 0;  // Pode por direto o nr. do Regisrador
			mux[M4].selecao   = 0;  // Pode por direto o nr. do Regisrador
			mux[M5].selecao   = MUX_3;
			mux[M6].selecao   = ULA;

			// -----------------------------
			estado=BUSCA;
			break;

		case BUSCA:
			// ----- Ciclo de Busca: --------
			//IR = memoria[PC];

			mux[M1].selecao = PC;
			RW = 0;
			ir.load = true;
			pc.incrementa = true;

			// ----------- -- ---------------
			estado=DECODIFICA;
			break;

		case DECODIFICA:

			// Case das instrucoes
			opcode = pega_pedaco(ir.valor, 15, 10);

			switch(opcode){
				case INCHAR:
					if(esta_teclando()) {
						tecla = getchar();
					}
					else {
						tecla = TECLA_PADRAO;
					}

					tecla = pega_pedaco(tecla,7,0);
					mux[M2].selecao = TECLADO;
					registradores[rx].load = true;

					// -----------------------------
					estado=BUSCA;
					break;

				case OUTCHAR:
					printf("%c", registradores[rx].valor);
					// -----------------------------
					estado=BUSCA;
					break;

				case LOADN:
					// reg[rx] = mem[PC];
					// PC++;
					mux[M1].selecao = PC;
					RW = 0;
					mux[M2].selecao = DADO_SAIDA;
					registradores[rx].load = true;
					pc.incrementa = true;
					// -----------------------------
					estado=BUSCA;
					break;

				case LOAD:
					// MAR = memoria[PC];
					// PC++;
					mux[M1].selecao = PC;
					RW = 0;
					mar.load = true;
					pc.incrementa = true;
					// -----------------------------
					estado=EXECUTA_1;
					break;

				case LOADI:
					// reg[rx] = memoria[reg[ry]];
					
					// -----------------------------
					estado=BUSCA;
					break;

				case STORE:
					//MAR = memoria[PC];
					//PC++;
					
					// -----------------------------
					estado=EXECUTA_1;
					break;

				case STOREI:
					//mem[reg[rx]] = reg[ry];
					
					// -----------------------------
					estado=BUSCA;
					break;

				case MOV:
					
					// -----------------------------
					estado=BUSCA;
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
					// reg[rx] = reg[ry] + reg[rz]; // Soma ou outra operacao
					
					// -----------------------------
					estado=BUSCA;
					break;

				case INC:
					//reg[rx]++;                                  // Inc Rx ou DEC
					
					// -----------------------------
					estado=BUSCA;
					break;

				case CMP:   // seta 3 flags: maior, menor ou igual
					//if(rx == ry)
					
					// -----------------------------
					estado=BUSCA;
					break;

				case SHIFT:

					switch(pega_pedaco(ir.valor, 6, 4))
					{
						case 0: 
							registradores[rx].valor = registradores[rx].valor << pega_pedaco(ir.valor, 3, 0);
							break;
						
						case 1:
							registradores[rx].valor = ~(~(registradores[rx].valor) << pega_pedaco(ir.valor, 3, 0));
							break;
						
						case 2:
							registradores[rx].valor = registradores[rx].valor >> pega_pedaco(ir.valor, 3, 0);
							break;
						
						case 3:
							registradores[rx].valor = ~(~(registradores[rx].valor) >> pega_pedaco(ir.valor,3,0));
							break;
						
						default:
								if(pega_pedaco(ir.valor, 6, 5) == 2) { // ROTATE LEFT
									registradores[rx].valor = rotaciona_esquerda(registradores[rx].valor, pega_pedaco(ir.valor, 3, 0));
								}
								else {
									registradores[rx].valor = rotaciona_direita(registradores[rx].valor, pega_pedaco(ir.valor, 3, 0)); 
								}
								break;
					}
					fr.flags[3] = 0; // -- FR = <...|zero|equal|lesser|greater>
					if(registradores[rx].valor == 0) {
						fr.flags[3] = 1;  // Se resultado = 0, seta o Flag de Zero
					}

					// -----------------------------
					estado=BUSCA;
					break;

				case JMP:
					COND = pega_pedaco(ir.valor, 9, 6);

					if((COND == 0)                       	                      // NO COND
							|| (fr.flags[0]==1 && (COND==7))                            // GREATER
							|| ((fr.flags[2]==1 || fr.flags[0]==1) && (COND==9))              // GREATER EQUAL
							|| (fr.flags[1]==1 && (COND==8))                            // LESSER
							|| ((fr.flags[2]==1 || fr.flags[1]==1) && (COND==10))             // LESSER EQUAL
							|| (fr.flags[2]==1 && (COND==1))                            // EQUAL
							|| (fr.flags[2]==0 && (COND==2))                            // NOT EQUAL
							|| (fr.flags[3]==1 && (COND==3))                            // ZERO
							|| (fr.flags[3]==0 && (COND==4))                            // NOT ZERO
							|| (fr.flags[4]==1 && (COND==5))                            // CARRY
							|| (fr.flags[4]==0 && (COND==6))                            // NOT CARRY
							|| (fr.flags[5]==1 && (COND==11))                           // OVERFLOW
							|| (fr.flags[5]==0 && (COND==12))                           // NOT OVERFLOW
							|| (fr.flags[6]==1 && (COND==14))                           // NEGATIVO
							|| (fr.flags[9]==1 && (COND==13)))                          // DIVBYZERO
					{ // PC = memoria[PC];
						mux[M1].selecao = PC;
						RW = 0;
						pc.load = true;
					}
					else
						//PC++;
						pc.incrementa = true;
					// -----------------------------
					estado=BUSCA;
					break;

				case CALL:
					
					estado=BUSCA;
					// -----------------------------
					break;

				case PUSH:
					
					// -----------------------------
					estado=BUSCA;
					break;

				case POP:
					//SP++;
					
					// -----------------------------
					estado=EXECUTA_1;
					break;

				case RTS:
					// SP++;
					
					// -----------------------------
					estado=EXECUTA_1;
					break;

				case SETC:
					fr.flags[4] = pega_pedaco(ir.valor, 9, 9);
					// -----------------------------
					estado=BUSCA;
					break;

				case HALT:        
					// -----------------------------
					estado=ESPERA;
					break;

				case NOP:         
					// -----------------------------
					estado=BUSCA;
					break;

				case BREAKP: 
					key = getchar(); 
					// -----------------------------
					estado=BUSCA;
					break;

				default:

					estado=BUSCA;
					break;
			}
			// -----------------------------
			//estado=EXECUTA_1;
			break;

		case EXECUTA_1:
			// -----------------------------
			switch(opcode){
				case LOAD:
					//reg[rx] = memoria[MAR];
					mux[M1].selecao = MAR;
					RW = 0;
					mux[M2].selecao = DADO_SAIDA;
					registradores[rx].load = true;
					// -----------------------------
					estado=BUSCA;
					break;

				case STORE:
					//memoria[MAR] = reg[rx];
					mux[M1].selecao = MAR;
					RW = 1;
					mux[M3].selecao = rx;
					mux[M5].selecao = MUX_3;
					// -----------------------------
					estado=BUSCA;
					break; 

				case CALL:
					mux[M1].selecao = PC;
					RW = 0;
					pc.load = true;
					// -----------------------------
					estado=BUSCA;
					break; 

				case POP:
					
					// -----------------------------
					estado=BUSCA;
					break; 

				case RTS:
					//PC = memoria[SP];
					
					// -----------------------------
					estado=EXECUTA_2;
					break;

				case PUSH:
					
					// -----------------------------
					estado=BUSCA;
					break;


			}

			//estado=EXECUTA_2;
			break;

		case EXECUTA_2:

			//case RTS:
			//PC++;
			
			// -----------------------------
			estado=BUSCA;
			break;

		case ESPERA:
			printf("\n");
			key = getchar();
			if (key == 'r') goto inicio;
			if (key == 'q') goto fim;
			break;

		default:
			break;
	}

	// Selecao do Mux4   --> Tem que vir antes do M1 e do M2 que usam M4
	if(mux[M4].selecao == 8) mux[M4].valor = 1;  // Seleciona 1 para fazer INC e DEC
	else mux[M4].valor = registradores[mux[M4].selecao].valor; 

	// Selecao do Mux1
	if      (mux[M1].selecao == PC)  mux[M1].valor = pc.valor;
	else if (mux[M1].selecao == MAR) mux[M1].valor = mar.valor;
	else if (mux[M1].selecao == MUX_4)  mux[M1].valor = mux[M4].valor;
	else if (mux[M1].selecao == SP)  mux[M1].valor = sp.valor;

	if(mux[M1].valor > TAMANHO_MEMORIA) {
		printf("  \n\nUltrapassou limite da memoria, coloque um jmp no fim do código\n");
		exit(1);
	}

	// Operacao de Leitura da Memoria
	if (RW == 0) DATA_OUT = memoria[mux[M1].valor];  // Tem que vir antes do M2 que usa DATA_OUT

	// Selecao do Mux3  --> Tem que vir antes da ULA e do M5
	if(mux[M3].selecao == FR) mux[M3].valor = flags_para_inteiro(fr.flags);
	else mux[M3].valor = registradores[mux[M3].selecao].valor; 

	// Operacao da ULA
	resultadoUla = ula(mux[M3].valor, mux[M4].valor, fr.flags[CARRY], carry, OP);

	// Selecao do Mux2
	if      (mux[M2].selecao == ULA) mux[M2].valor = resultadoUla.valor;
	else if (mux[M2].selecao == DADO_SAIDA) mux[M2].valor = DATA_OUT;
	else if (mux[M2].selecao == MUX_4)  mux[M2].valor = mux[M4].valor;
	//else if (selM2 == sTECLADO) M2 = TECLADO;// TODO: selM2 com teclado
	else if (mux[M2].selecao == SP)  mux[M2].valor = sp.valor;

	// Selecao do Mux5
	if (mux[M5].selecao == PC) mux[M5].valor = pc.valor;
	else if (mux[M5].selecao == MUX_3) mux[M5].valor = mux[M3].valor;

	// Selecao do Mux6
	if (mux[M6].selecao == ULA) mux[M6].valor = resultadoUla.fr;// TODO: Talvez o auxFR deva ser o valor do FR //**Sempre recebe flags da ULA
	else if (mux[M6].selecao == DADO_SAIDA) mux[M6].valor = DATA_OUT; //** A menos que seja POP FR, quando recebe da Memoria

	goto loop;

fim:
	return 0;
}
