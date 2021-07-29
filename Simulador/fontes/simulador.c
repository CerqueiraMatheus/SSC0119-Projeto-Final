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

#define TAMANHO_MEMORIA 32768

// Estados do Processador
#define STATE_RESET 0
#define STATE_FETCH 1
#define STATE_DECODE 2
#define STATE_EXECUTE 3
#define STATE_EXECUTE2 4
#define STATE_HALTED 5
//----------------

// Selecao do Mux1
#define sPC 0
#define sMAR 1
#define sM4 2
#define sSP 3

// Selecao do Mux2
#define sULA 0
#define sDATA_OUT 1
//#define sM4 2
//#define sSP 3
#define sTECLADO 4

// Selecao do Mux3 --> E´ so´ colocar: 0, 1, 2 ... 7  para selecionar os Registradores

// Selecao do Mux4 --> E´ so´ colocar: 0, 1, 2 ... 7  para selecionar os Registradores ou 8 para entrar o nr. 1

// Selecao do Mux5
//#define sPC 0
#define sM3 1

// Selecao do Mux6
//#define sULA 0
//#define sDATA_OUT 1

// Opcodes das Instrucoes:
// Data Manipulation:
#define LOADIMED 56   // "111000"; -- LOAD Rx Nr  (b0=0)   -- Rx <- Nr    ou   Load SP Nr (b0=1)  -- SP <- Nr    Format: < inst(6) | Rx(3) | xxxxxxb0 >  + 16bit Numero
#define LOADINDEX 60  // "111100"; -- LOAD Rx Ry   -- Rx <- M[Ry]	Format: < inst(6) | Rx(3) | Ry(3) | xxxx >
#define STOREINDEX 61 // "111101"; -- STORE Rx Ry  -- M[Rx] <- Ry	Format: < inst(6) | Rx(3) | Ry(3) | xxxx >


#define LMOD 37     // "100101"; -- MOD Rx Ry Rz   			-- Rx <- Ry MOD Rz 	  	Format: < inst(6) | Rx(3) | Ry(3) | Rz(3)| x >


// Logic Instructions (All should begin wiht "01"):
#define LAND 18     // "010010"; -- AND Rx Ry Rz  	-- Rz <- Rx AND Ry	Format: < inst(6) | Rx(3) | Ry(3) | Rz(3)| x >
#define LOR 19      // "010011"; -- OR Rx Ry Rz   	-- Rz <- Rx OR Ry		Format: < inst(6) | Rx(3) | Ry(3) | Rz(3)| x >
#define LXOR 20     // "010100"; -- XOR Rx Ry Rz  	-- Rz <- Rx XOR Ry	Format: < inst(6) | Rx(3) | Ry(3) | Rz(3)| x >
#define LNOT 21     // "010101"; -- NOT Rx Ry       	-- Rx <- NOT(Ry)		Format: < inst(6) | Rx(3) | Ry(3) | xxxx >


// Flag register
#define NEGATIVE 9
#define STACK_UNDERFLOW 8
#define STACK_OVERFLOW 7
#define DIV_BY_ZERO 6
#define ARITHMETIC_OVERFLOW 5
#define CARRY 4
#define ZERO 3
#define EQUAL 2
#define LESSER 1
#define GREATER 0

//#include <curses.h>     //  Novo Terminal cheio de funcoes!!!
#include <stdlib.h>     // Rand
#include <stdio.h>      // Printf
#include <fcntl.h>      // Fileopen - Fileclose - fprintf - fscanf
#include <math.h>

// kbhit() TODO: deletar
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "operacao.h"
#include "ula.h"

unsigned int MEMORY[TAMANHO_MEMORIA]; // Vetor que representa a Memoria de programa e de dados do Processador
int reg[8]; // 8 registradores


//  Processa dados do Arquivo CPU.MIF
void le_arquivo(void);

//processa uma linha completa e retorna o número codificado
int processa_linha(char* linha); 


int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}

int FR[16] = {0};  // Flag Register: <...|Negativo|StackUnderflow|StackOverflow|DivByZero|ArithmeticOverflow|carry|zero|equal|lesser|greater>

int main()
{
	int i=0;
	int key=0;    // Le Teclado
	int PC=0, IR=0, SP=0, MAR=0, rx=0, ry=0, rz=0, COND=0, RW=0, DATA_OUT=0;
	int LoadPC=0, IncPC=0, LoadIR=0, LoadSP=0, IncSP=0, DecSP=0, LoadMAR=0, LoadFR=0;
	int M1=0, M2=0, M3=0, M4=0, M5=0, M6=0;
	int selM1=0, selM2=0, selM3=0, selM4=0, selM5=0, selM6=0;
	int LoadReg[8] = {0};
	int carry=0;// Flag do IR que indica se a ULA vai fazer operação com carry ou não 
	int opcode=0;
	int temp=0;
	unsigned char state=0; // reset
	int OP=0;  // ULA
	int TECLADO;
	resultado_ula_t resultadoUla;

	le_arquivo();

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
	if(LoadIR) IR = DATA_OUT;

	if(LoadPC) PC = DATA_OUT;

	if(IncPC) PC++;

	if(LoadMAR) MAR = DATA_OUT;

	if(LoadSP) SP = M4;

	if(IncSP) SP++;

	if(DecSP) SP--;

	if(LoadFR)
		for(i=16; i--; )              // Converte o int M6 para o vetor FR
			FR[i] = pega_pedaco(M6,i,i); //  Tem que trasformar em Vetor

	// Carrega dados do Mux 2 para os registradores
	rx = pega_pedaco(IR,9,7);
	ry = pega_pedaco(IR,6,4);
	rz = pega_pedaco(IR,3,1);
	
	// Coloca valor do Mux2 para o registrador com Load
	if(LoadReg[rx]) reg[rx] = M2;

	// Operacao de Escrita da Memoria
	if (RW == 1) MEMORY[M1] = M5;

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
				FR[i] = 0;

			PC = 0;  // inicializa na linha Zero da memoria -> Programa tem que comecar na linha Zero !!
			IR = 0;
			MAR = 0;
			SP = TAMANHO_MEMORIA -1;

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
			selM1   = sPC;
			selM2   = sDATA_OUT;
			selM3   = 0;  // Pode por direto o nr. do Regisrador
			selM4   = 0;  // Pode por direto o nr. do Regisrador
			selM5   = sM3;
			selM6   = sULA;

			// -----------------------------
			state=STATE_FETCH;
			break;

		case STATE_FETCH:
			// ----- Ciclo de Busca: --------
			//IR = MEMORY[PC];

			selM1 = sPC;
			RW = 0;
			LoadIR = 1;
			IncPC = 1;

			// ----------- -- ---------------
			state=STATE_DECODE;
			break;

		case STATE_DECODE:

			// Case das instrucoes
			opcode = pega_pedaco(IR,15,10);

			switch(opcode){
				case INCHAR:
					// TODO: entrada teclado
					//TECLADO = getchar();
					//timeout(99999);
					//if(TECLADO == ERR)
					//	TECLADO = 255;
					if(kbhit())
						TECLADO = getchar();
					else
						TECLADO = 255;

					TECLADO = pega_pedaco(TECLADO,7,0);
					selM2 = sTECLADO;
					LoadReg[rx] = 1;

					// -----------------------------
					state=STATE_FETCH;
					break;

				case OUTCHAR:
					printf("%c", reg[rx]);
					// -----------------------------
					state=STATE_FETCH;
					break;

				case LOADIMED:
					// reg[rx] = mem[PC];
					// PC++;
					selM1 = sPC;
					RW = 0;
					selM2 = sDATA_OUT;
					LoadReg[rx] = 1;
					IncPC = 1;
					// -----------------------------
					state=STATE_FETCH;
					break;

				case LOAD:
					// MAR = MEMORY[PC];
					// PC++;
					selM1 = sPC;
					RW = 0;
					LoadMAR = 1; 
					IncPC = 1;
					// -----------------------------
					state=STATE_EXECUTE;
					break;

				case LOADINDEX:
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

				case STOREINDEX:
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
				case LMOD:
				case LAND:
				case LOR:
				case LXOR:
				case LNOT:
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

					switch(pega_pedaco(IR,6,4))
					{   case 0: reg[rx] = reg[rx] << pega_pedaco(IR,3,0);           break;
						case 1: reg[rx] = ~((~(reg[rx]) << pega_pedaco(IR,3,0)));   break;
						case 2: reg[rx] = reg[rx] >> pega_pedaco(IR,3,0);           break;
						case 3: reg[rx] = ~((~(reg[rx]) >> pega_pedaco(IR,3,0)));   break;
						default:
								if(pega_pedaco(IR,6,5)==2) // ROTATE LEFT
									reg[rx] = rotaciona_esquerda(reg[rx],pega_pedaco(IR,3,0));
								else
									reg[rx] = rotaciona_direita(reg[rx],pega_pedaco(IR,3,0)); 
								break;
					}
					FR[3] = 0; // -- FR = <...|zero|equal|lesser|greater>
					if(reg[rx] == 0)
						FR[3] = 1;  // Se resultado = 0, seta o Flag de Zero

					// -----------------------------
					state=STATE_FETCH;
					break;

				case JMP:
					COND = pega_pedaco(IR,9,6);

					if((COND == 0)                       	                      // NO COND
							|| (FR[0]==1 && (COND==7))                            // GREATER
							|| ((FR[2]==1 || FR[0]==1) && (COND==9))              // GREATER EQUAL
							|| (FR[1]==1 && (COND==8))                            // LESSER
							|| ((FR[2]==1 || FR[1]==1) && (COND==10))             // LESSER EQUAL
							|| (FR[2]==1 && (COND==1))                            // EQUAL
							|| (FR[2]==0 && (COND==2))                            // NOT EQUAL
							|| (FR[3]==1 && (COND==3))                            // ZERO
							|| (FR[3]==0 && (COND==4))                            // NOT ZERO
							|| (FR[4]==1 && (COND==5))                            // CARRY
							|| (FR[4]==0 && (COND==6))                            // NOT CARRY
							|| (FR[5]==1 && (COND==11))                           // OVERFLOW
							|| (FR[5]==0 && (COND==12))                           // NOT OVERFLOW
							|| (FR[6]==1 && (COND==14))                           // NEGATIVO
							|| (FR[9]==1 && (COND==13)))                          // DIVBYZERO
					{ // PC = MEMORY[PC];
						selM1 = sPC;
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
					FR[4] = pega_pedaco(IR,9,9);
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
					selM1 = sMAR;
					RW = 0;
					selM2 = sDATA_OUT;
					LoadReg[rx] = 1;
					// -----------------------------
					state=STATE_FETCH;
					break;

				case STORE:
					//MEMORY[MAR] = reg[rx];
					selM1 = sMAR;
					RW = 1;
					selM3 = rx;
					selM5 = sM3;
					// -----------------------------
					state=STATE_FETCH;
					break; 

				case CALL:
					selM1 = sPC;
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
	if(selM4 == 8) M4 = 1;  // Seleciona 1 para fazer INC e DEC
	else M4 = reg[selM4]; 

	// Selecao do Mux1
	if      (selM1 == sPC)  M1 = PC;
	else if (selM1 == sMAR) M1 = MAR;
	else if (selM1 == sM4)  M1 = M4;
	else if (selM1 == sSP)  M1 = SP;

	if(M1 > (TAMANHO_MEMORIA)) {
		M1 = 0;
		printf("  \n\nUltrapassou limite da memoria, coloque um jmp no fim do código\n");
		exit(1);
	}

	// Operacao de Leitura da Memoria
	if (RW == 0) DATA_OUT = MEMORY[M1];  // Tem que vir antes do M2 que usa DATA_OUT

	// Selecao do Mux3  --> Tem que vir antes da ULA e do M5
	// Converte o vetor FR para int
	// TODO talvez fazer isso depois da operação da ula?
	temp = 0;
	for(i=16; i--; )        
		temp = temp + (int) (FR[i] * (pow(2.0,i))); 

	if(selM3 == 8) M3 = temp;  // Seleciona com 8 o FR
	else M3 = reg[selM3]; 

	// Operacao da ULA
	resultadoUla = ula(M3, M4, FR[CARRY], carry, OP);

	// Selecao do Mux2
	if      (selM2 == sULA) M2 = resultadoUla.valor;
	else if (selM2 == sDATA_OUT) M2 = DATA_OUT;
	else if (selM2 == sM4)  M2 = M4;
	//else if (selM2 == sTECLADO) M2 = TECLADO;// TODO: selM2 com teclado
	else if (selM2 == sSP)  M2 = SP; 

	// Selecao do Mux5
	if (selM5 == sPC) M5 = PC;
	else if (selM5 == sM3) M5 = M3;

	// Selecao do Mux6
	if (selM6 == sULA) M6 = resultadoUla.fr;// TODO: Talvez o auxFR deva ser o valor do FR //**Sempre recebe flags da ULA
	else if (selM6 == sDATA_OUT) M6 = DATA_OUT; //** A menos que seja POP FR, quando recebe da Memoria

	goto loop;

fim:
	return 0;
}

//  Processa dados do Arquivo CPU.MIF
void le_arquivo(void){
	FILE *stream;   // Declara ponteiro para o arquivo
	int i, j;
	int processando = 0; // Flag para varreo o arquivo CPURAM.mif e tirar o cabecalho

	if ( (stream = fopen("cpuram.mif","r")) == NULL)  // Abre o arquivo para leitura
	{
		printf("Error: File not OPEN!");
		exit(1);
	}

	char linha[110];
	j = 0;

	while (fscanf(stream,"%s", linha)!=EOF)   // Le linha por linha ate' o final do arquivo: eof = end of file !!
	{
		char letra[2] = "00";

		if (!processando) {
			i = 0;
			do  {   // Busca por sequencias de caracteres para catar inicio do codigo
				letra[0] = letra[1];
				letra[1] = linha[i];
				if ((letra[0]=='0') && (letra[1]==':') )  // Cata primeira linha de codigo omecando com "0: "
				{
					// Le programa e guarda no vetor MEMORY[32768]
					processando = 1;
					j = 0;
				}
				i++;
			} while (linha[i] != '\0');

		}

		if (processando && (j < TAMANHO_MEMORIA)) {
			MEMORY[j] = processa_linha(linha);
			if (MEMORY[j] == -1) {
				printf("Linha invalida (%d): '%s'", j, linha);
			}
			else {
				//printf("Valor: %d. Linha: %s\n", MEMORY[j], linha);
			}
			j++;
		}



	} // Fim do   while (!feof(stream))
	fclose(stream);  // Nunca esqueca um arquivo aberto!!
}

//processa uma linha completa e retorna o número codificado
//retorna -1 em caso de erro
//NOTA: Assume radix=BIN no arquivo CPURAM.MIF
int processa_linha(char* linha) {
	int i=0;
	int j=0;
	int valor=0;
	//procura o inicio do numero
	while (linha[i] != ':') {
		if (linha[i] == 0) {
			return -1;
		}
		i++;
	}

	valor = 0;
	for (j=0;j<16;j++) { //le a palavra toda
		valor <<= 1; //shifta pra esquerda entrando 0
		valor += linha[i+j+1] - '0'; //converte char pra numero 0 ou 1 o bit 15-k da palavra
	}

	return valor;
}
