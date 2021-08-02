// gcc simple_simulator.c -O3 -march=native -o simulador -Wall -lm
// -lm is option to execute math.h library file.
/*
Perguntas:
1) O que tenho que fazer?
2) Onde começa?
3) Onde Termina?
4) Qual é o caminho?
Do todos os comandos...
5) Acabou??
6) E o PC ????????
*/

// Estados do Processador
#define STATE_RESET 0
#define STATE_FETCH 1
#define STATE_DECODE 2
#define STATE_EXECUTE 3
#define STATE_EXECUTE2 4
#define STATE_HALTED 5
//----------------

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "operacao.h"
#include "memoria.h"
#include "teclado.h"
#include "flag.h"
#include "ula.h"
#include "mux.h"


unsigned int MEMORY[TAMANHO_MEMORIA]; // Vetor que representa a Memoria de programa e de dados do Processador
int reg[8]; // 8 registradores

int fr[16] = {0};  // Flag Register: <...|Negativo|StackUnderflow|StackOverflow|DivByZero|ArithmeticOverflow|carry|zero|equal|lesser|greater>

int main()
{
	int i=0;
	int key=0;    // Le Teclado
	int pc=0, ir=0, sp=0, mar=0, rx=0, ry=0, rz=0, COND=0, RW=0, DATA_OUT=0;
	int LoadPC=0, IncPC=0, LoadIR=0, LoadSP=0, IncSP=0, DecSP=0, LoadMAR=0, LoadFR=0;

	mux_t mux[NUMERO_MUX];
	for (int i = 0; i < NUMERO_MUX; i++) {
		mux[i].dado = 0;
		mux[i].selecao = 0;
	}

	int LoadReg[8] = {0};
	int carry=0;// Flag do IR que indica se a ULA vai fazer operação com carry ou não 
	int opcode=0;
	int temp=0;
	unsigned char state=0; // reset
	int OP=0;  // ULA
	int tecla;
	resultado_ula_t resultadoUla;

	le_arquivo_memoria(MEMORY);

inicio:
	printf("PROCESSADOR ICMC  - Menu:\n");
	printf("                          'r' goto inicio...\n");
	printf("                          'q' goto fim...\n\n");
	printf("Rodando...\n");

	state = STATE_RESET;

	// Loop principal do processador: Nunca para!!
loop:

	//key = getchar();   

	// Executa Load dos Registradores
	if(LoadIR) ir = DATA_OUT;

	if(LoadPC) pc = DATA_OUT;

	if(IncPC) pc++;

	if(LoadMAR) mar = DATA_OUT;

	if(LoadSP) sp = mux[M4].dado;

	if(IncSP) sp++;

	if(DecSP) sp--;

	if(LoadFR)
		for(i=16; i--; )              // Converte o int M6 para o vetor FR
			fr[i] = pega_pedaco(mux[M6].dado, i, i); //  Tem que trasformar em Vetor

	// Carrega dados do Mux 2 para os registradores
	rx = pega_pedaco(ir,9,7);
	ry = pega_pedaco(ir,6,4);
	rz = pega_pedaco(ir,3,1);
	
	// Coloca valor do Mux2 para o registrador com Load
	if(LoadReg[rx]) reg[rx] = mux[M2].dado;

	// Operacao de Escrita da Memoria
	if (RW == 1) MEMORY[mux[M1].dado] = mux[M5].dado;

	// ---------------------------------------

	// Reinicializa os Loads dos registradores
	for(i=0;i<8;i++)
		LoadReg[i] = 0;

	RW = 0;

	LoadIR  = 0;
	LoadMAR = 0;
	LoadPC  = 0;
	IncPC   = 0;
	LoadSP  = 0;
	IncSP   = 0;
	DecSP   = 0;
	LoadFR  = 0;


	// Maquina de Controle
	switch(state)
	{
		case STATE_RESET:
			// Reinicializa os registradores
			for(i=0;i<8;i++) {
				reg[i] = 0;
				LoadReg[i] = 0;
			}
			for(i=0;i<16;i++)
				fr[i] = 0;

			pc = 0;  // inicializa na linha Zero da memoria -> Programa tem que comecar na linha Zero !!
			ir = 0;
			mar = 0;
			sp = TAMANHO_MEMORIA -1;

			RW = 0;
			DATA_OUT = 0;  // Barramento de saida de Dados da Memoria

			LoadIR  = 0;
			LoadMAR = 0;
			LoadPC  = 0;
			IncPC   = 0;
			LoadSP  = 0;
			IncSP   = 0;
			DecSP   = 0;
			LoadFR  = 0;
			mux[M1].selecao   = PC;
			mux[M2].selecao   = DADO_SAIDA;
			mux[M3].selecao   = 0;  // Pode por direto o nr. do Regisrador
			mux[M4].selecao   = 0;  // Pode por direto o nr. do Regisrador
			mux[M5].selecao   = MUX_3;
			mux[M6].selecao   = ULA;

			// -----------------------------
			state=STATE_FETCH;
			break;

		case STATE_FETCH:
			// ----- Ciclo de Busca: --------
			//IR = MEMORY[PC];

			mux[M1].selecao = PC;
			RW = 0;
			LoadIR = 1;
			IncPC = 1;

			// ----------- -- ---------------
			state=STATE_DECODE;
			break;

		case STATE_DECODE:

			// Case das instrucoes
			opcode = pega_pedaco(ir,15,10);

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
					LoadReg[rx] = 1;

					// -----------------------------
					state=STATE_FETCH;
					break;

				case OUTCHAR:
					printf("%c", reg[rx]);
					// -----------------------------
					state=STATE_FETCH;
					break;

				case LOADN:
					// reg[rx] = mem[PC];
					// PC++;
					mux[M1].selecao = PC;
					RW = 0;
					mux[M2].selecao = DADO_SAIDA;
					LoadReg[rx] = 1;
					IncPC = 1;
					// -----------------------------
					state=STATE_FETCH;
					break;

				case LOAD:
					// MAR = MEMORY[PC];
					// PC++;
					mux[M1].selecao = PC;
					RW = 0;
					LoadMAR = 1; 
					IncPC = 1;
					// -----------------------------
					state=STATE_EXECUTE;
					break;

				case LOADI:
					// reg[rx] = MEMORY[reg[ry]];
					
					// -----------------------------
					state=STATE_FETCH;
					break;

				case STORE:
					//MAR = MEMORY[PC];
					//PC++;
					
					// -----------------------------
					state=STATE_EXECUTE;
					break;

				case STOREI:
					//mem[reg[rx]] = reg[ry];
					
					// -----------------------------
					state=STATE_FETCH;
					break;

				case MOV:
					
					// -----------------------------
					state=STATE_FETCH;
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
					state=STATE_FETCH;
					break;

				case INC:
					//reg[rx]++;                                  // Inc Rx ou DEC
					
					// -----------------------------
					state=STATE_FETCH;
					break;

				case CMP:   // seta 3 flags: maior, menor ou igual
					//if(rx == ry)
					
					// -----------------------------
					state=STATE_FETCH;
					break;

				case SHIFT:

					switch(pega_pedaco(ir,6,4))
					{   case 0: reg[rx] = reg[rx] << pega_pedaco(ir,3,0);           break;
						case 1: reg[rx] = ~((~(reg[rx]) << pega_pedaco(ir,3,0)));   break;
						case 2: reg[rx] = reg[rx] >> pega_pedaco(ir,3,0);           break;
						case 3: reg[rx] = ~((~(reg[rx]) >> pega_pedaco(ir,3,0)));   break;
						default:
								if(pega_pedaco(ir,6,5)==2) // ROTATE LEFT
									reg[rx] = rotaciona_esquerda(reg[rx],pega_pedaco(ir,3,0));
								else
									reg[rx] = rotaciona_direita(reg[rx],pega_pedaco(ir,3,0)); 
								break;
					}
					fr[3] = 0; // -- FR = <...|zero|equal|lesser|greater>
					if(reg[rx] == 0)
						fr[3] = 1;  // Se resultado = 0, seta o Flag de Zero

					// -----------------------------
					state=STATE_FETCH;
					break;

				case JMP:
					COND = pega_pedaco(ir,9,6);

					if((COND == 0)                       	                      // NO COND
							|| (fr[0]==1 && (COND==7))                            // GREATER
							|| ((fr[2]==1 || fr[0]==1) && (COND==9))              // GREATER EQUAL
							|| (fr[1]==1 && (COND==8))                            // LESSER
							|| ((fr[2]==1 || fr[1]==1) && (COND==10))             // LESSER EQUAL
							|| (fr[2]==1 && (COND==1))                            // EQUAL
							|| (fr[2]==0 && (COND==2))                            // NOT EQUAL
							|| (fr[3]==1 && (COND==3))                            // ZERO
							|| (fr[3]==0 && (COND==4))                            // NOT ZERO
							|| (fr[4]==1 && (COND==5))                            // CARRY
							|| (fr[4]==0 && (COND==6))                            // NOT CARRY
							|| (fr[5]==1 && (COND==11))                           // OVERFLOW
							|| (fr[5]==0 && (COND==12))                           // NOT OVERFLOW
							|| (fr[6]==1 && (COND==14))                           // NEGATIVO
							|| (fr[9]==1 && (COND==13)))                          // DIVBYZERO
					{ // PC = MEMORY[PC];
						mux[M1].selecao = PC;
						RW = 0;
						LoadPC = 1;
					}
					else
						//PC++;
						IncPC = 1;
					// -----------------------------
					state=STATE_FETCH;
					break;

				case CALL:
					
					state=STATE_FETCH;
					// -----------------------------
					break;

				case PUSH:
					
					// -----------------------------
					state=STATE_FETCH;
					break;

				case POP:
					//SP++;
					
					// -----------------------------
					state=STATE_EXECUTE;
					break;

				case RTS:
					// SP++;
					
					// -----------------------------
					state=STATE_EXECUTE;
					break;

				case SETC:
					fr[4] = pega_pedaco(ir,9,9);
					// -----------------------------
					state=STATE_FETCH;
					break;

				case HALT:        
					// -----------------------------
					state=STATE_HALTED;
					break;

				case NOP:         
					// -----------------------------
					state=STATE_FETCH;
					break;

				case BREAKP: 
					key = getchar(); 
					// -----------------------------
					state=STATE_FETCH;
					break;

				default:

					state=STATE_FETCH;
					break;
			}
			// -----------------------------
			//state=STATE_EXECUTE;
			break;

		case STATE_EXECUTE:
			// -----------------------------
			switch(opcode){
				case LOAD:
					//reg[rx] = MEMORY[MAR];
					mux[M1].selecao = MAR;
					RW = 0;
					mux[M2].selecao = DADO_SAIDA;
					LoadReg[rx] = 1;
					// -----------------------------
					state=STATE_FETCH;
					break;

				case STORE:
					//MEMORY[MAR] = reg[rx];
					mux[M1].selecao = MAR;
					RW = 1;
					mux[M3].selecao = rx;
					mux[M5].selecao = MUX_3;
					// -----------------------------
					state=STATE_FETCH;
					break; 

				case CALL:
					mux[M1].selecao = PC;
					RW = 0;
					LoadPC = 1;
					// -----------------------------
					state=STATE_FETCH;
					break; 

				case POP:
					
					// -----------------------------
					state=STATE_FETCH;
					break; 

				case RTS:
					//PC = MEMORY[SP];
					
					// -----------------------------
					state=STATE_EXECUTE2;
					break;

				case PUSH:
					
					// -----------------------------
					state=STATE_FETCH;
					break;


			}

			//state=STATE_EXECUTE2;
			break;

		case STATE_EXECUTE2:

			//case RTS:
			//PC++;
			
			// -----------------------------
			state=STATE_FETCH;
			break;

		case STATE_HALTED:
			printf("\n");
			key = getchar();
			if (key == 'r') goto inicio;
			if (key == 'q') goto fim;
			break;

		default:
			break;
	}

	// Selecao do Mux4   --> Tem que vir antes do M1 e do M2 que usam M4
	if(mux[M4].selecao == 8) mux[M4].dado = 1;  // Seleciona 1 para fazer INC e DEC
	else mux[M4].dado = reg[mux[M4].selecao]; 

	// Selecao do Mux1
	if      (mux[M1].selecao == PC)  mux[M1].dado = pc;
	else if (mux[M1].selecao == MAR) mux[M1].dado = mar;
	else if (mux[M1].selecao == MUX_4)  mux[M1].dado = mux[M4].dado;
	else if (mux[M1].selecao == SP)  mux[M1].dado = sp;

	if(mux[M1].dado > TAMANHO_MEMORIA) {
		printf("  \n\nUltrapassou limite da memoria, coloque um jmp no fim do código\n");
		exit(1);
	}

	// Operacao de Leitura da Memoria
	if (RW == 0) DATA_OUT = MEMORY[mux[M1].dado];  // Tem que vir antes do M2 que usa DATA_OUT

	// Selecao do Mux3  --> Tem que vir antes da ULA e do M5
	// Converte o vetor FR para int
	// TODO talvez fazer isso depois da operação da ula?
	temp = 0;
	for(i=16; i--; )        
		temp = temp + (int) (fr[i] * (pow(2.0,i))); 

	if(mux[M3].selecao == FR) mux[M3].dado = temp;  // Seleciona com 8 o FR
	else mux[M3].dado = reg[mux[M3].selecao]; 

	// Operacao da ULA
	resultadoUla = ula(mux[M3].dado, mux[M4].dado, fr[CARRY], carry, OP);

	// Selecao do Mux2
	if      (mux[M2].selecao == ULA) mux[M2].dado = resultadoUla.valor;
	else if (mux[M2].selecao == DADO_SAIDA) mux[M2].dado = DATA_OUT;
	else if (mux[M2].selecao == MUX_4)  mux[M2].dado = mux[M4].dado;
	//else if (selM2 == sTECLADO) M2 = TECLADO;// TODO: selM2 com teclado
	else if (mux[M2].selecao == SP)  mux[M2].dado = sp;

	// Selecao do Mux5
	if (mux[M5].selecao == PC) mux[M5].dado = pc;
	else if (mux[M5].selecao == MUX_3) mux[M5].dado = mux[M3].dado;

	// Selecao do Mux6
	if (mux[M6].selecao == ULA) mux[M6].dado = resultadoUla.fr;// TODO: Talvez o auxFR deva ser o valor do FR //**Sempre recebe flags da ULA
	else if (mux[M6].selecao == DADO_SAIDA) mux[M6].dado = DATA_OUT; //** A menos que seja POP FR, quando recebe da Memoria

	goto loop;

fim:
	return 0;
}
