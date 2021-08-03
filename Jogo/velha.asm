; Jogo da Velha
jmp main



jogador : var #1



main:
	call cria_jogador
	call troca_jogador
	call troca_jogador
	load r0, jogador
	loadn r1, #0
	outchar r0, r1

	halt



cria_jogador:
	push r0
	
	loadn r0, #'X'				;
	store jogador, r0			; jogador := 'X'
	
	pop r0
	rts


troca_jogador:
	push r0
	push r1
	
	load r0, jogador			;
	loadn r1, #'X'				; caractere := 'X'
	cmp r0, r1					;
	jne troca_jogador__fim		; se jogador = caractere:
	loadn r1, #'O'				;     caractere := 'O'
								;
troca_jogador__fim:				;
	store jogador, r1			; jogador := caractere
	
	pop r1
	pop r0
	rts
