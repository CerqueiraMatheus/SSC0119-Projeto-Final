; Jogo da Velha
jmp main


jogador : var #1
tabuleiro : var #9


main:
	call le_caractere
	call caractere_para_digito
	loadn r1, #0
	loadn r2, #'A'
	add r0, r0, r2
	outchar r0, r1

	halt


; JOGADOR

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
	loadn r1, #'X'					; caractere := 'X'
	cmp r0, r1						;
	jne troca_jogador__fim			; se jogador = caractere:
	loadn r1, #'O'					;     caractere := 'O'
									;
troca_jogador__fim:					;
	store jogador, r1				; jogador := caractere
	
	pop r1
	pop r0
	pop fr
	rts


; TABULEIRO

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


; CARACTERE

; Retorno:
; r0 - caractere
le_caractere:
	push fr
	push r1
	
	loadn r1, #255					; VAZIO := 255
									;
le_caractere__loop:					; faça:
	inchar r0						;     caractere := entrada()
	cmp r0, r1						;
	jeq le_caractere__loop			; enquanto caractere = VAZIO
	
	pop r1
	pop fr
	rts

; Parâmetros:
; r0 - caractere
; Retorno:
; r0 - digito
caractere_para_digito:
	push fr
	push r1
	
	loadn r1, #'0'					;
	sub r0, r0, r1					; retorna caractere - '0'
	
	pop r1
	pop fr
	rts
