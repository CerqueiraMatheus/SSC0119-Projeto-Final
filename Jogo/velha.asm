; Jogo da Velha
jmp main


jogador : var #1
tabuleiro : var #9


main:
	call cria_tabuleiro
	
	loadn r0, #tabuleiro
	loadn r1, #8
	add r0, r0, r1
	
	loadi r2, r0
	loadn r3, #0
	outchar r2, r3

	halt


; Jogador

cria_jogador:
	push r0
	
	loadn r0, #'X'					;
	store jogador, r0				; jogador := 'X'
	
	pop r0
	rts

troca_jogador:
	push r0
	push r1
	
	load r0, jogador				;
	loadn r1, #'X'					; caractere := 'X'
	cmp r0, r1						;
	jne troca_jogador__fim			; se jogador = caractere:
	loadn r1, #'O'					;     caractere := 'O'
									;
troca_jogador__fim:					;
	store jogador, r1				; jogador := caractere
	
	pop r1
	pop r0
	rts


; Tabuleiro

cria_tabuleiro:
	push r0
	push r1
	push r2
	
	loadn r0, #'A'					;
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
	rts
