; Jogo da Velha


main:
	call cria_pontos
	
main__inicio:
	call cria_rodada
	call cria_jogador
	call cria_tabuleiro
	
	call imprime_tela
	call imprime_pontos
	
	load r0, NENHUM

main__loop:
	call imprime_jogador
	
	call executa_jogada
	call imprime_posicao
	
	call checa_vencedor
	load r1, vencedor
	cmp r0, r1
	jne main__fim
	
	call proxima_rodada
	call troca_jogador
	jmp main__loop

main__fim:
	call imprime_vencedor
	call atualiza_pontos

	call le_caractere
	jmp main__inicio


; Caractere

le_caractere:
	push fr
	push r0
	push r1
	
	load r0, NAO_DIGITADO					;
											;
le_caractere__loop:							; repita:
	inchar r1								;     caractere := entrada()
	cmp r0, r1								;
	jeq le_caractere__loop					; enquanto caractere = NAO_DIGITADO
											;
	store caractere, r1						;
	
	pop r1
	pop r0
	pop fr
	rts


; Digito

checa_digito:
	push fr
	push r0
	push r1
	push r2
	
	load r0, FALSO							; 
	load r1, caractere						;
											;
	loadn r2, #'1'							;
	cmp r1, r2								; se caractere < '1':
	jle checa_digito__fim					;     dígito_válido := FALSO
											;
	loadn r2, #'9'							; senão se caractere > '9':
	cmp r1, r2								;     dígito_válido := FALSO
	jgr checa_digito__fim					;
											; senão:
	load r0, VERDADEIRO						;     dígito_válido := VERDADEIRO
											;
checa_digito__fim:							;
	store digito_valido, r0					;
	
	pop r2
	pop r1
	pop r0
	pop fr
	rts


caractere_para_digito:
	push fr
	push r0
	push r1
	
	load r0, caractere						;
	loadn r1, #'0'							;
	sub r0, r0, r1							;
	store digito, r0						; dígito := caractere - '0'
	
	pop r1
	pop r0
	pop fr
	rts


; Sprite

carrega_sprite:
	push fr
	push r0
	push r1
	
	load r0, jogador						;
	load r1, X								;
	cmp r0, r1								;
	jne carrega_sprite__o					; se jogador = X:
											;
	load r0, SPRITE_X						;     sprite := SPRITE_X
	jmp carrega_sprite__fim					;
											;
carrega_sprite__o:							; senão:
	load r0, SPRITE_O						;     sprite := SPRITE_Y
											;
carrega_sprite__fim:						;
	store sprite, r0						;
	
	pop r1
	pop r0
	pop fr
	rts


carrega_cor:
	push fr
	push r0
	push r1
	
	load r0, jogador						;
	load r1, X								;
	cmp r0, r1								;
	jne carrega_cor__o						; se jogador = X:
											;
	load r0, VERMELHO						;     cor := VERMELHO
	jmp carrega_cor__fim					;
											;
carrega_cor__o:								; senão
	load r0, AZUL							;     cor := AZUL
											;
carrega_cor__fim:							;
	store cor, r0							;
	
	pop r1
	pop r0
	pop fr
	rts


; Posição

checa_posicao:
	push fr
	push r0
	push r1
	push r2
	
	load r0, FALSO							; posição_válida := FALSO
											;
	loadn r1, #tabuleiro					;
	load r2, posicao						;
	add r1, r1, r2							;
	loadi r1, r1							; casa := tabuleiro[posição]
											;
	load r2, VAZIO							;
	cmp r1, r2								; se casa != VAZIO:
	jne checa_posicao__fim					;     posição_válida := FALSO
											; senão:
	load r0, VERDADEIRO						;     posição_válida := VERDADEIRO
	
checa_posicao__fim:
	store posicao_valida, r0
	
	pop r2
	pop r1
	pop r0
	pop fr
	rts


digito_para_posicao:
	push fr
	push r0
	
	load r0, digito							;
	dec r0									;
	store posicao, r0						; posição := dígito - 1
	
	pop r0
	pop fr
	rts


imprime_posicao:
	push fr
	push r0
	push r1
	push r2
	push r3
	
	call carrega_sprite						; sprite := carrega_sprite(jogador)
	load r0, sprite							;
											;
	call carrega_cor						; cor := carrega_cor(jogador)
	load r1, cor							;
	add r0, r0, r1							;
											;
	loadn r1, #POSICAO_PARA_COORDENADA		;
	load r2, posicao						;
	add r1, r1, r2							;
	loadi r1, r1							; coordenada := POSIÇÃO_PARA_COORDENADA[posição]
											;
	loadn r2, #37							;
	loadn r3, #160							;
	add r3, r3, r1							; última := coordenada + 160
											;
imprime_posicao__loop:						; repita:
	outchar r0, r1							;     saída(sprite + cor, coordenada)
	inc r0									;     sprite++
	inc r1									;     coordenada++
											;
	outchar r0, r1							;     saída(sprite + cor, coordenada)
	inc r0									;     sprite++
	inc r1									;     coordenada++
											;
	outchar r0, r1							;     saída(sprite + cor, coordenada)
	inc r0									;     sprite++
	inc r1									;     coordenada++
											;
	outchar r0, r1							;     saída(sprite + cor, coordenada)
	inc r0									;     sprite++
											;
	add r1, r1, r2							;     coordenada += 37
	cmp r1, r3								;
	jle imprime_posicao__loop				; enquanto coordenada < última
	
	pop r3
	pop r2
	pop r1
	pop r0
	pop fr
	rts


; Jogador

cria_jogador:
	push fr
	push r0
	
	load r0, X								;
	store jogador, r0						; jogador := X
	
	pop r0
	pop fr
	rts


troca_jogador:
	push fr
	push r0
	push r1
	
	load r0, jogador						;
	load r1, X								; novo := X
	cmp r0, r1								;
	jne troca_jogador__fim					; se jogador = novo:
											;
	load r1, O								;     novo := O
											;
troca_jogador__fim:							;
	store jogador, r1						; jogador := novo
	
	pop r1
	pop r0
	pop fr
	rts


imprime_jogador:
	push fr
	push r0
	push r1
	
	load r0, jogador						;
	load r1, O								;
	cmp r0, r1								;
	jeq imprime_jogador__o					; se jogador = X:
											;
	load r1, VERMELHO						;     cor := VERMELHO
											;
	jmp imprime_jogador__fim				;
											;
imprime_jogador__o:							; senão:
	load r1, AZUL							;     cor := AZUL
											;
imprime_jogador__fim:						;
	add r0, r0, r1							;
	load r1, COORDENADA_JOGADOR				;
	outchar r0, r1							; saída(jogador + cor, COORDENADA_JOGADOR)
	
	pop r1
	pop r0
	pop fr
	rts


; Tabuleiro

cria_tabuleiro:
	push fr
	push r0
	push r1
	push r2
	
	load r0, VAZIO							;
	loadn r1, #tabuleiro					; casa := &tabuleiro
											;
	load r2, POSICOES						;
	add r2, r1, r2							; última := casa + POSIÇÕES
											;
cria_tabuleiro__loop:						; repita:
	storei r1, r0							;     *casa := VAZIO
	inc r1									;     casa++
	cmp r1, r2								;
	jle cria_tabuleiro__loop				; enquanto casa < última
	
	pop r2
	pop r1
	pop r0
	pop fr
	rts


preenche_tabuleiro:
	push fr
	push r0
	push r1
	
	loadn r0, #tabuleiro					;
	load r1, posicao						;
	add r0, r0, r1							; casa := &tabuleiro[posição]
											;
	load r1, jogador						;
	storei r0, r1							; *casa = jogador
	
	pop r1
	pop r0
	pop fr
	rts


; Rodada

cria_rodada:
	push fr
	push r0
	
	loadn r0, #1							;
	store rodada, r0						; rodada := 1
	
	pop r0
	pop fr
	rts


proxima_rodada:
	push fr
	push r0
	
	load r0, rodada							;
	inc r0									;
	store rodada, r0						; rodada++
	
	pop r0
	pop fr
	rts


; Pontos

cria_pontos:
	push fr
	push r0
	
	loadn r0, #0							;
	store pontos_x, r0						; pontos_x := 0
	store pontos_o, r0						; pontos_o := 0
	
	pop r0
	pop fr
	rts


atualiza_pontos:
	push fr
	push r0
	push r1
	push r2
	push r3

	load r0, pontos_x						;
	load r1, pontos_o						;
	load r2, vencedor						;
											;
	load r3, X								;
	cmp r2, r3								;
	jne atualiza_pontos__o					; se vencedor = X:
											;
	inc r0									;     pontos_x++
	jmp atualiza_pontos__fim				;
											;
atualiza_pontos__o:							;
	load r3, O								;
	cmp r2, r3								;
	jne atualiza_pontos__fim				; senão se vencedor = O
											;
	inc r1									;
											;     pontos_o++
atualiza_pontos__fim:						;
	store pontos_x, r0						;
	store pontos_o, r1						;

	pop r3
	pop r2
	pop r1
	pop r0
	pop fr
	rts


; Parâmetros:
; r0 - pontos
pontos_para_string:
	push fr
	push r0
	push r1
	push r2
	push r3
	push r4
	push r5
	
	loadn r1, #10							;
	loadn r2, #'0'							;
											;
	loadn r3, #string_pontos				;
	loadn r4, #2							;
	add r4, r3, r4							; letra := &string_pontos[2]
											;
pontos_para_string__loop:					; repita:
	mod r5, r0, r1							;
	add r5, r5, r2							;
	storei r4, r5							;     *letra := pontos % 10 + '0'
											;
	div r0, r0, r1							;     pontos /= 10
											;
	dec r4									;     letra--
	cmp r4, r3								;
	jeg pontos_para_string__loop			; enquanto letra >= string_pontos
	
	pop r5
	pop r4
	pop r3
	pop r2
	pop r1
	pop r0
	pop fr
	rts


imprime_pontos:
	push fr
	push r0
	push r1
	push r2
	
	load r0, pontos_x						;
	call pontos_para_string					; string_pontos := pontos_para_string(pontos_x)
											;
	loadn r0, #string_pontos				;
	load r1, COORDENADA_PONTOS_X			;
	load r2, VERMELHO						;
	call imprime_string						; imprime_string(string_pontos, COORDENADA_PONTOS_X, VERMELHO)
											;
	load r0, pontos_o						;
	call pontos_para_string					; string_pontos := pontos_para_string(pontos_o)
											;
	loadn r0, #string_pontos				;
	load r1, COORDENADA_PONTOS_O			;
	load r2, AZUL							;
	call imprime_string						; imprime_string(string_pontos, COORDENADA_PONTOS_Y, AZUL)
	
	pop r2
	pop r1
	pop r0
	pop fr
	rts


; Jogada

executa_jogada:
	push fr
	push r0
	push r1
	
	load r0, VERDADEIRO						;
											;
executa_jogada__loop:						; repita:
	call le_caractere						;     caractere := le_caractere()
											;
	call checa_digito						;     dígito_válido := checa_dígito(caracter)
	load r1, digito_valido					;
	cmp r0, r1								;
	jne executa_jogada__loop				;     se dígito_válido = FALSO:
											;         continue
	call caractere_para_digito				;     dígito := caracter_para_dígito(caracter)
	call digito_para_posicao				;     posição := dígito_para_posção(dígito)
											;
	call checa_posicao						;     posição_válida := checa_posição(tabuleiro, posição)
	load r1, posicao_valida					;
	cmp r0, r1								;
	jne executa_jogada__loop				;     se posição_válida = VERDADEIRO:
											;         saia
	call preenche_tabuleiro					; preenche_tabuleiro(tabuleiro, posição, jogador)
	
	pop r1
	pop r0
	pop fr
	rts


; Vencedor

checa_linhas:
	push fr
	push r0
	push r1
	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	
	load r0, NENHUM							; vencedor := NENHUM
	load r1, VAZIO							;
	load r2, POSICOES						;
	loadn r3, #3							;
	loadn r4, #0							; i := 0
											;
											;
checa_linhas__loop:							; repita:
	loadn r5, #tabuleiro					;
	add r5, r5, r4							;
											;
	loadi r6, r5							;     primeira := tabuleiro[i]
	cmp r1, r6								;     se primeira = VAZIO:
	jeq checa_linhas__inc					;         continue
											;
	inc r5									;
	loadi r7, r5							;     segunda := tabuleiro[i + 1]
	cmp r6, r7								;     se primeira != segunda:
	jne checa_linhas__inc					;         continue
											;
	inc r5									;
	loadi r7, r5							;     terceira := tabuleiro[i + 2]
	cmp r6, r7								;     se primeira != terceira:
	jne checa_linhas__inc					;         continue
											;
	mov r0, r6								;     vencedor := primeira
											;
checa_linhas__inc:							;
	add r4, r4, r3							;     i += 3
	cmp r4, r2								;
	jle checa_linhas__loop					; enquanto i < POSIÇÕES
											;
	store vencedor, r0						;
	
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
	pop r1
	pop r0
	pop fr
	rts


checa_colunas:
	push fr
	push r0
	push r1
	push r2
	push r3
	push r4
	push r5
	push r6
	
	load r0, NENHUM							; vencedor := NENHUM
	load r1, VAZIO							;
	loadn r2, #3							;
	loadn r3, #0							; i = 0
											;
checa_colunas__loop:						; repita:
	loadn r4, #tabuleiro					;
	add r4, r4, r3							;
											;
	loadi r5, r4							;     primeira := tabuleiro[i]
	cmp r1, r5								;     se primeira = VAZIO:
	jeq checa_colunas__inc					;         continue
											;
	add r4, r4, r2							;
	loadi r6, r4							;     segunda := tabuleiro[i + 3]
	cmp r5, r6								;     se primeira != segunda:
	jne checa_colunas__inc					;         continue
											;
	add r4, r4, r2							;
	loadi r6, r4							;     terceira := tabuleiro[i + 6]
	cmp r5, r6								;     se primeira != terceira:
	jne checa_colunas__inc					;         continue
											;
	mov r0, r5								;     vencedor := primeira
											;
checa_colunas__inc:							;
	inc r3									;     i++
	cmp r3, r2								;
	jle checa_colunas__loop					; enquanto i < 3
											;
	store vencedor, r0						;
	
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2
	pop r1
	pop r0
	pop fr
	rts


checa_diagonal_1:
	push fr
	push r0
	push r1
	push r2
	push r3
	push r4
	
	load r0, NENHUM							; vencedor := NENHUM
	loadn r1, #4							;
	loadn r2, #tabuleiro					;
											;
	loadi r3, r2							; primeira := tabuleiro[0]
	cmp r0, r3								; se primeira = VAZIO:
	jeq checa_diagonal_1__fim				;     saia
											;
	add r2, r2, r1							;
	loadi r4, r2							; segunda := tabuleiro[4]
	cmp r3, r4								; se primeira != segunda:
	jne checa_diagonal_1__fim				;     saia
											;
	add r2, r2, r1							;
	loadi r4, r2							; terceira := tabuleiro[8]
	cmp r3, r4								; se primeira != terceira:
	jne checa_diagonal_1__fim				;     saia
											;
	mov r0, r3 								; vencedor := primeira
											;
checa_diagonal_1__fim:						;
	store vencedor, r0						;
	
	pop r4
	pop r3
	pop r2
	pop r1
	pop r0
	pop fr
	rts


checa_diagonal_2:
	push fr
	push r0
	push r1
	push r2
	push r3
	push r4
	
	load r0, NENHUM							; vencedor := NENHUM
	loadn r1, #2							;
	loadn r2, #tabuleiro					;
											;
	add r2, r2, r1							;
	loadi r3, r2							; primeira := tabuleiro[2]
	cmp r0, r3								; se primeira = VAZIO:
	jeq checa_diagonal_2__fim				;     saia
											;
	add r2, r2, r1							;
	loadi r4, r2							; segunda := tabuleiro[4]
	cmp r3, r4								; se primeira != segunda:
	jne checa_diagonal_2__fim				;     saia
											;
	add r2, r2, r1							;
	loadi r4, r2							; terceira := tabuleiro[6]
	cmp r3, r4								; se primeira != terceira:
	jne checa_diagonal_2__fim				;     saia
											;
	mov r0, r3 								; vencedor := primeira
											;
checa_diagonal_2__fim:						;
	store vencedor, r0						;
	
	pop r4
	pop r3
	pop r2
	pop r1
	pop r0
	pop fr
	rts


checa_rodada:
	push fr
	push r0
	push r1
	push r2
	
	load r0, NENHUM							; vencedor := NENHUM
	load r1, RODADAS						;
											;
	load r2, rodada							;
	cmp r1, r2								;
	jne checa_rodada__fim					; se rodada = RODADAS:
											;
	load r0, EMPATE							;     vencedor := EMPATE
											;
checa_rodada__fim:							;
	store vencedor, r0						;

	pop r2
	pop r1
	pop r0
	pop fr
	rts

checa_vencedor:
	push fr
	push r0
	push r1
	
	load r0, NENHUM							;
											;
	call checa_linhas						; vencedor := checa_linhas(tabuleiro)
	load r1, vencedor						;
	cmp r0, r1								;
	jne checa_vencedor__fim					; se vencedor != NENHUM:
											;
	call checa_colunas						;     vencedor := checa_colunas(tabuleiro)
	load r1, vencedor						;
	cmp r0, r1								;
	jne checa_vencedor__fim					; se vencedor != NENHUM:
											;
	call checa_diagonal_1					;     vencedor := checa_diagonal_1(tabuleiro)
	load r1, vencedor						;
	cmp r0, r1								;
	jne checa_vencedor__fim					; se vencedor != NENHUM:
											;
	call checa_diagonal_2					;     vencedor := checa_diagonal_2(tabuleiro)
	load r1, vencedor						;
	cmp r0, r1								;
	jne checa_vencedor__fim					; se vencedor != NENHUM:
											;
	call checa_rodada						;     vencedor := checa_rodada(tabuleiro)
	
checa_vencedor__fim:
	pop r1
	pop r0
	pop fr
	rts


imprime_vencedor:
	push fr
	push r0
	push r1
	push r2
	
	load r0, vencedor						;
	load r1, EMPATE							;
	cmp r0, r1								;
	jeq imprime_vencedor__empate			; se vencedor != EMPATE:
											;
	call carrega_cor						;     cor := carrega_cor(jogador)
	load r1, cor							;
	add r0, r0, r1							;
											;
	load r1, COORDENADA_VENCEDOR			;
	outchar r0, r1							;     saída(vencedor + cor, COORDENADA_VENCEDOR)
											;
	loadn r0, #STRING_VENCEDOR				;
	loadn r2, #2							;
	add r1, r1, r2							;
	load r2, BRANCO							;
	call imprime_string						;     imprime_string(STRING_VENCEDOR, COORDENADA_VENCEDOR + 2, BRANCO)
											;
	jmp imprime_vencedor__fim				;
											;
imprime_vencedor__empate:					; senão:
	loadn r0, #STRING_EMPATE				;
	load r1, COORDENADA_VENCEDOR			;
	load r2, BRANCO							;
	call imprime_string						;     imprime_string(STRING_EMPATE, COORDENADA_VENCEDOR, BRANCO)

imprime_vencedor__fim:
	pop r2
	pop r1
	pop r0
	pop fr
	rts


; Tela

; Parâmetros:
; r0 - string
; r1 - coordenada
; r2 - cor
imprime_string:
	push fr
	push r0
	push r1
	push r2
	push r3
	push r4
	
	loadn r3, #'\0'							;
											;
imprime_string__loop:						; enquanto *string != '\0':
	loadi r4, r0							;
	cmp r3, r4								;
	jeq imprime_string__fim					;
											;
	add r4, r4, r2							;
	outchar r4, r1							;     saída(*string + cor, coordenada)
											;
	inc r0									;     string++
	inc r1									;     coordenada++
	jmp imprime_string__loop				;
	
imprime_string__fim:
	pop r4
	pop r3
	pop r2
	pop r1
	pop r0
	pop fr
	rts


imprime_tela:
	push fr
	push r0
	push r1
	push r2
	push r3
	push r4
	push r5
	
	loadn r0, #TELA_0						; string := &TELA_0
	loadn r1, #0							; coordenada := 0
	load r2, BRANCO							;
											;
	load r3, TAMANHO_TELA					;
	loadn r4, #41							;
	loadn r5, #40							;
											;
imprime_tela__loop:							; repita:
	call imprime_string						;     imprime_string(string, coordenada, cor)
											;
	add r0, r0, r4							;     string += 41
	add r1, r1, r5							;     coordenada += 40
											;
	cmp r1, r3								;
	jle imprime_tela__loop					; enquanto coordenada < TAMANHO_TELA

imprime_tela__fim:
	pop r5
	pop r4
	pop r3
	pop r2
	pop r1
	pop r0
	pop fr
	rts


; Variáveis

cor : var #1
rodada : var #1
jogador : var #1
vencedor : var #1
pontos_x : var #1
pontos_o : var #1
tabuleiro : var #9

string_pontos : var #4
static string_pontos + #3, #'\0'

sprite : var #1
digito : var #1
posicao : var #1
caractere : var #1
digito_valido : var #1
posicao_valida : var #1


; Constantes

X : var #1
static X, #'X'

O : var #1
static O, #'O'

VAZIO : var #1
static VAZIO, #' '


EMPATE : var #1
static EMPATE, #'-'

NENHUM : var #1
static NENHUM, #' '


RODADAS : var #1
static RODADAS, #9

POSICOES : var #1
static POSICOES, #9


NAO_DIGITADO : var #1
static NAO_DIGITADO, #255


FALSO : var #1
static FALSO, #0

VERDADEIRO : var #1
static VERDADEIRO, #1


BRANCO : var #1
static BRANCO, #0

VERMELHO : var #1
static VERMELHO, #2304

AZUL : var #1
static AZUL, #3072


SPRITE_X : var #1
static SPRITE_X, #0

SPRITE_O : var #1
static SPRITE_O, #16


STRING_VENCEDOR : string "venceu! Tecle para rejogar"
STRING_EMPATE : string "Eh velha! Tecle para rejogar"


COORDENADA_JOGADOR : var #1
static COORDENADA_JOGADOR, #132

COORDENADA_PONTOS_X : var #1
static COORDENADA_PONTOS_X, #187

COORDENADA_PONTOS_O : var #1
static COORDENADA_PONTOS_O, #193

COORDENADA_VENCEDOR : var #1
static COORDENADA_VENCEDOR, #1166

POSICAO_PARA_COORDENADA : var #9
static POSICAO_PARA_COORDENADA + #0, #368
static POSICAO_PARA_COORDENADA + #1, #378
static POSICAO_PARA_COORDENADA + #2, #388
static POSICAO_PARA_COORDENADA + #3, #648
static POSICAO_PARA_COORDENADA + #4, #658
static POSICAO_PARA_COORDENADA + #5, #668
static POSICAO_PARA_COORDENADA + #6, #928
static POSICAO_PARA_COORDENADA + #7, #938
static POSICAO_PARA_COORDENADA + #8, #948


TAMANHO_TELA : var #1
static TAMANHO_TELA, #1200

TELA_0  : string "             Jogo da Velha!             "
TELA_1  : string "                                        "
TELA_2  : string "  +-----------+          +-----------+  "
TELA_3  : string "  | Vez do:   |          | Pontuacao |  "
TELA_4  : string "  |           |          |     -     |  "
TELA_5  : string "  +-----------+          +-----------+  "
TELA_6  : string "                                        "
TELA_7  : string "                                        "
TELA_8  : string "              ||        ||              "
TELA_9  : string "              ||        ||              "
TELA_10 : string "              ||        ||              "
TELA_11 : string "              ||        ||              "
TELA_12 : string "              ||        ||              "
TELA_13 : string "            1 ||      2 ||      3       "
TELA_14 : string "      ========++========++========      "
TELA_15 : string "              ||        ||              "
TELA_16 : string "              ||        ||              "
TELA_17 : string "              ||        ||              "
TELA_18 : string "              ||        ||              "
TELA_19 : string "              ||        ||              "
TELA_20 : string "            4 ||      5 ||      6       "
TELA_21 : string "      ========++========++========      "
TELA_22 : string "              ||        ||              "
TELA_23 : string "              ||        ||              "
TELA_24 : string "              ||        ||              "
TELA_25 : string "              ||        ||              "
TELA_26 : string "              ||        ||              "
TELA_27 : string "            7 ||      8 ||      9       "
TELA_28 : string "                                        "
TELA_29 : string "                                        "
