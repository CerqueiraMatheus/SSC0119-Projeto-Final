; Jogo da Velha
jmp main


jogador : var #1
posicao : var #1
tabuleiro : var #9

caractere : var #1
eh_digito : var #1
digito : var #1


main:
	call le_caractere
	call checa_digito
	
	load r0, eh_digito
	loadn r1, #'0'
	add r0, r0, r1
	
	loadn r1, #0
	outchar r0, r1

	halt


; Jogador

cria_jogador:
	push fr
	push r0
	
	loadn r0, #'X'					;
	store jogador, r0				; jogador := 'X'
	
	pop r0
	pop fr
	rts


troca_jogador:
	push fr
	push r0
	push r1
	
	load r0, jogador				;
	loadn r1, #'X'					; novo := 'X'
	cmp r0, r1						;
	jne troca_jogador__fim			; se jogador = novo:
	loadn r1, #'O'					;     novo := 'O'
									;
troca_jogador__fim:					;
	store jogador, r1				; jogador := novo
	
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
	
	loadn r0, #' '					;
	loadn r1, #tabuleiro			; posição := tabuleiro
	loadn r2, #9					;
	add r2, r2, r1					; última := posição + 9
									;
cria_tabuleiro__loop:				; faça:
	storei r1, r0					;     *posição := ' '
	inc r1							;     posição++
	cmp r1, r2						;
	jle cria_tabuleiro__loop		; enquanto posição < última
	
	pop r2
	pop r1
	pop r0
	pop fr
	rts


; Caractere

le_caractere:
	push fr
	push r0
	push r1
	
	loadn r1, #255					; VAZIO := 255
									;
le_caractere__loop:					; faça:
	inchar r0						;     caractere := entrada()
	cmp r0, r1						;
	jeq le_caractere__loop			; enquanto caractere = VAZIO
									;
	store caractere, r0				;
	
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
	
	loadn r0, #0					; 
	load r1, caractere				;
									;
	loadn r2, #'0'					;
	cmp r1, r2						; se caractere < '0':
	jle checa_digito__fim			;     eh_digito := falso
									;
	loadn r2, #'9'					; senão se caractere > '9':
	cmp r1, r2						;     eh_digito := falso
	jgr checa_digito__fim			;
									; senão:
	loadn r0, #1					;     eh_digito := verdadeiro
									;
checa_digito__fim:					;
	store eh_digito, r0				;
	
	pop r2
	pop r1
	pop r0
	pop fr
	rts


caractere_para_digito:
	push fr
	push r0
	push r1
	
	load r0, caractere				;
	loadn r1, #'0'					;
	sub r0, r0, r1					;
	store digito, r0				; digito := caractere - '0'
	
	pop r1
	pop r0
	pop fr
	rts

