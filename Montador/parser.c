#include "parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>  //calcula o tamanho do terminal

#include "defs.h"
#include "structs.h"

extern FILE *out; /* Codigo de saida */
extern FILE *in;  /* Codigo de entrada */
extern char *OutputFileName;

char Look;          /* Ultimo caractere lido */
int curr_pos = 0;   /* Posicao atual */
int curr_line = 1;  /* Linha atual */
char *progr_buffer; /* String completa do programa */
long progr_size;
char progr_buffer_out[MEM_SIZE][17]; /* Buffer de saida */
int col_count;                       //quantidade de colunas no terminal

//prototipos locais
long parser_GetFileSize();

//-------------------------------------------------------------------------------------------------------------
//funcoes que iniciam o montador

void calcula_tamanho() {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    col_count = w.ws_col;
}

long parser_GetFileSize() {
    long curr = ftell(in), ret;
    if (fseek(in, 0L, SEEK_END)) parser_Abort("erro indefinido ao calcular o tamanho do arquivo");
    ret = ftell(in);
    if (fseek(in, curr, SEEK_SET)) parser_Abort("erro indefinido ao calcular o tamanho do arquivo");
    return ret;
}

int CarregaPrograma(char *nome) {
    in = fopen(nome, "r");
    if (in == NULL) parser_Abort("Nao foi possivel carregar o arquivo de programa.");

    progr_size = parser_GetFileSize();
    if (progr_size > MAX_BUFF_SIZE) parser_Abort("Programa grande demais para caber em memoria! Tamanho maximo 977Kb");

    progr_buffer = (char *)malloc((progr_size + 1) * sizeof(char));

    if (progr_buffer == NULL) parser_Abort("Memoria insuficiente para buffer do programa!");

    int i;

    //    for (i=0;i<=MAX_BUFF_SIZE;i++) progr_buffer[i]='\0';
    //    memset eh mais otimizado

    memset(progr_buffer, 0, progr_size);

    i = 0;
    while (!feof(in)) {
        progr_buffer[i] = fgetc(in);
        i++;
    }

    fclose(in);

    return i;
}

void parser_Rewind(void) {
    curr_pos = 0;
    curr_line = 1; /* Contagem de linhas nao e precisa. */
}

/* Inicializa buffer de saida */
void parser_init_out_buffer(void) {
    int i;

    for (i = 0; i < MEM_SIZE; i++)
        strcpy(progr_buffer_out[i], "0000000000000000");
}

void parser_Init(void) {
    parser_Rewind();
    parser_GetChar();
    parser_SkipWhite();
    parser_SkipComment();
    parser_SkipWhite();
}

//-----------------------------------------------------------------------------------------------------------
//funcoes que interagem com a tela

void parser_GetAttention(char *s) {
    int sz = strlen(s), i;
    for (i = 0; i < col_count / (sz + 1); i++)
        printf("%s ", s);
    printf("\n");
}

void parser_Message(char *s) {
    printf("Mensagem (%d): %s\n", curr_line, s);
}

void parser_Warning(char *s) {
    printf("Aviso (%d): %s\n", curr_line, s);
}

void parser_Error(char *s) {
    parser_GetAttention("ERRO");
    printf("\nErro (%d): %s\n", curr_line, s);
}

void parser_Write(char *texto) {
    fprintf(out, "%s\n", texto);
    //    printf("Escrito : %s\n",texto);
}

void parser_Halt(void) {
#ifndef __linux__
    system("pause");
#endif
    exit(1);
}

void parser_Abort(char *s) {
    char ss[STRTAM];
    sprintf(ss, "rm %s", OutputFileName);
    system(ss);
    parser_Error(s);
    parser_Halt();
}

//-----------------------------------------------------------------------------
//funcoes de comparacao

int parser_IsAlNum(char c) {
    return (isalpha(c) || isdigit(c) || c == '.' || c == '-' || c == '_');
}

int parser_IsWhite(char c) {
    return (c == ' ' || c == TAB || c == '\n' || c == '\r');
}

int parser_IsInt(char *s) {
    int i = 0;
    for (i = 0; i < strlen(s); i++)
        if (!isdigit(s[i]) && s[i] != '-') {
            parser_Abort("Inteiro esperado");
            return FALSE;
        }
    return TRUE;
}

//---------------------------------------------------------------------------
//funcoes que mexem no codigo

void parser_GetChar(void) {
    Look = progr_buffer[curr_pos];
    if (Look == '\n' || Look == '\r') curr_line++;
    if (Look == '\0') Look = EOF;
    curr_pos++;
}

void parser_Match(char x) {
    if (Look != x) {
        char ss[STRTAM];
        sprintf(ss, "\'%c\' esperado", x);
        parser_Abort(ss);
    } else {
        parser_GetChar();
        parser_SkipWhite();
        parser_SkipComment();
        parser_SkipWhite();
    }
}

void parser_Match_noskip(char x) {
    if (Look != x) {
        char ss[STRTAM];
        sprintf(ss, "\'%c\' esperado", x);
        parser_Abort(ss);
    } else {
        parser_GetChar();
    }
}
void parser_SkipWhite(void) {
    while (parser_IsWhite(Look)) parser_GetChar();
}

void parser_SkipComment(void) {
    while (Look == ';') {
        parser_GetChar();
        parser_SkipUntilEnd();
    }
}

void parser_SkipUntil(char c) {
    while (Look != c) {
        parser_GetChar();
        if (Look == EOF) parser_Match(c); /* Match sempre falha, entao da erro se chegar no EOF */
    }
    parser_Match(c);
    parser_SkipWhite();
    parser_SkipComment();
    parser_SkipWhite();
}

void parser_SkipUntilEnd(void) {
    while (Look != '\n' && Look != '\r') {
        parser_GetChar();
        if (Look == EOF) break;
    }
    parser_SkipWhite();
    parser_SkipComment();
    parser_SkipWhite();
}

/* Pega nome de label, registrador ou intrucao */
char *parser_GetItem_s(void) {
    char *Token;
    Token = (char *)calloc(STRTAM, sizeof(char));
    int i = 0;
    if (!parser_IsAlNum(Look)) parser_Abort("Label, Instrucao ou Registrador esperado");
    while (parser_IsAlNum(Look)) {
        Token[i] = Look;
        parser_GetChar();
        i++;
        if (i > LABEL_MAX_SIZE - 1) parser_Abort("Menos caracteres esperados");
    }
    Token[i] = '\0';
    parser_SkipWhite();
    parser_SkipComment();
    parser_SkipWhite();
    return Token;
}

/* Pega Numero (como string) */
char *parser_GetNum_s(void) {
    char *Value;
    Value = (char *)calloc(STRTAM, sizeof(char));
    int i = 0;
    if (!isdigit(Look)) parser_Abort("Numero esperado");
    while (isdigit(Look)) {
        Value[i] = Look;
        parser_GetChar();
        i++;
        if (i > STRTAM - 1) parser_Abort("Menos caracteres esperados");
    }
    Value[i] = '\0';
    parser_SkipWhite();
    parser_SkipComment();
    parser_SkipWhite();
    return Value;
}

void parser_Write_Inst(char word[17], unsigned int end) {
    /*
    fprintf(out,"%d:%.16s;\n",end,word);
    */
    if (end >= MEM_SIZE) {
        parser_Abort("Limite da memoria ultrapassado!");
    }
    strcpy(progr_buffer_out[end], word);
    //    printf("Escrito : %.16s em 0x%x\n",word,end);
}

void parser_str_to_upper(char *str) {
    register unsigned int i = 0;
    while (i < strlen(str)) {
        str[i] = toupper(str[i]);
        i = i + 1;
    }
}

/* Descarrega buffer de saida no arquivo */
void parser_flush_program(void) {
    int i;

    for (i = 0; i < MEM_SIZE; i++)
        fprintf(out, "%d:%.16s;\n", i, progr_buffer_out[i]);
}
