; Jogo da Velha
jmp main


jogador : var #1

tabuleiro : var #9

posicao_valida : var #1
posicao : var #1

caractere : var #1
digito_valido : var #1
digito : var #1

vencedor : var #1

rodada : var #1


main:
	call cria_rodada
	call cria_jogador
	call cria_tabuleiro
	
	loadn r0, #' '

main__loop:
	call executa_jogada
	
	call checa_vencedor
	load r1, vencedor
	cmp r0, r1
	jne main__fim
	
	call proxima_rodada
	call troca_jogador
	jmp main__loop

main__fim:
	load r0, vencedor
	loadn r1, #0
	outchar r0, r1

	halt


; Rodada

cria_rodada:
	push fr
	push r0
	
	loadn r0, #1					;
	store rodada, r0				; rodada := 1
	
	pop r0
	pop fr
	rts


proxima_rodada:
	push fr
	push r0
	
	load r0, rodada					;
	inc r0							;
	store rodada, r0				; rodada++
	
	pop r0
	pop fr
	rts


; Jogo

executa_jogada:
	push fr
	push r0
	push r1
	
	loadn r0, #1					;
									;
executa_jogada__loop:				; repita:
	call le_caractere				;     caractere := le_caractere()
									;
	call checa_digito				;     digito_valido := checa_digito(caracter)
	load r1, digito_valido			;
	cmp r0, r1						;
	jne executa_jogada__loop		;     se digito_valido = falso:
									;         continue
	call caractere_para_digito		;     digito := caracter_para_digito(caracter)
	call digito_para_posicao		;     posicao := digito_para_posicao(digito)
									;
	call checa_posicao				;     posicao_valida := checa_posicao(tabuleiro, posicao)
	load r1, posicao_valida			;
	cmp r0, r1						;
	jne executa_jogada__loop		;     se posicao_valida = verdadeiro:
									;         saia
	call preenche_tabuleiro			; preenche_tabuleiro(tabuleiro, posicao, jogador)
	
	pop r1
	pop r0
	pop fr
	rts


checa_vencedor:
	push fr
	push r0
	push r1
	
	loadn r0, #' '
	
	call checa_linhas
	load r1, vencedor
	cmp r0, r1
	jne checa_vencedor__fim
	
	call checa_colunas
	load r1, vencedor
	cmp r0, r1
	jne checa_vencedor__fim
	
	call checa_diagonal_1
	load r1, vencedor
	cmp r0, r1
	jne checa_vencedor__fim
	
	call checa_diagonal_2
	load r1, vencedor
	cmp r0, r1
	jne checa_vencedor__fim
	
	call checa_rodada

checa_vencedor__fim:
	pop r1
	pop r0
	pop fr
	rts


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
	
	loadn r0, #' '					; vencedor := NENHUM
	loadn r1, #' '					; VAZIO := ' '
	loadn r2, #3					;
	loadn r3, #9					;
	loadn r4, #0					; i := 0
									;
									;
checa_linhas__loop:					; repita:
	loadn r5, #tabuleiro			;
	add r5, r5, r4					;
									;
	loadi r6, r5					;     primeira := tabuleiro[i]
	cmp r1, r6						;     se primeira = VAZIO:
	jeq checa_linhas__inc			;         continue
									;
	inc r5							;
	loadi r7, r5					;     segunda := tabuleiro[i + 1]
	cmp r6, r7						;     se primeira != segunda:
	jne checa_linhas__inc			;         continue
									;
	inc r5							;
	loadi r7, r5					;     terceira := tabuleiro[i + 2]
	cmp r6, r7						;     se primeira != terceira:
	jne checa_linhas__inc			;         continue
									;
	mov r0, r6						;     vencedor := primeira
									;
checa_linhas__inc:					;
	add r4, r4, r2					;     i += 3
	cmp r4, r3						;
	jle checa_linhas__loop			; enquanto i < 9
									;
	store vencedor, r0				;
	
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
	
	loadn r0, #' '					; vencedor := NENHUM
	loadn r1, #' '					; VAZIO := ' '
	loadn r2, #3					;
	loadn r3, #0					; i = 0
									;
checa_colunas__loop:				; repita:
	loadn r4, #tabuleiro			;
	add r4, r4, r3					;
									;
	loadi r5, r4					;     primeira := tabuleira[i]
	cmp r1, r5						;     se primeira = VAZIO:
	jeq checa_colunas__inc			;         continue
									;
	add r4, r4, r2					;
	loadi r6, r4					;     segunda := tabuleiro[i + 3]
	cmp r5, r6						;     se primeira != segunda:
	jne checa_colunas__inc			;         continue
									;
	add r4, r4, r2					;
	loadi r6, r4					;     terceira := tabuleiro[i + 6]
	cmp r5, r6						;     se primeira != terceira:
	jne checa_colunas__inc			;         continue
									;
	mov r0, r5						;     vencedor := primeira
									;
checa_colunas__inc:					;
	inc r3							;     i++
	cmp r3, r2						;
	jle checa_colunas__loop			; enquanto i < 3
									;
	store vencedor, r0				;
	
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
	
	loadn r0, #' '					; vencedor := NENHUM
	loadn r1, #4					;
	loadn r2, #tabuleiro			;
									;
	loadi r3, r2					; primeira := tabuleiro[0]
	cmp r0, r3						; se primeira = VAZIO:
	jeq checa_diagonal_1__fim		;     saia
									;
	add r2, r2, r1					;
	loadi r4, r2					; segunda := tabuleiro[4]
	cmp r3, r4						; se primeira != segunda:
	jne checa_diagonal_1__fim		;     saia
									;
	add r2, r2, r1					;
	loadi r4, r2					; terceira := tabuleiro[8]
	cmp r3, r4						; se primeira != terceira:
	jne checa_diagonal_1__fim		;     saia
									;
	mov r0, r3 						; vencedor := primeira
									;
checa_diagonal_1__fim:				;
	store vencedor, r0				;
	
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
	
	loadn r0, #' '					; vencedor := NENHUM
	loadn r1, #2					;
	loadn r2, #tabuleiro			;
									;
	add r2, r2, r1					;
	loadi r3, r2					; primeira := tabuleiro[2]
	cmp r0, r3						; se primeira = VAZIO:
	jeq checa_diagonal_2__fim		;     saia
									;
	add r2, r2, r1					;
	loadi r4, r2					; segunda := tabuleiro[4]
	cmp r3, r4						; se primeira != segunda:
	jne checa_diagonal_2__fim		;     saia
									;
	add r2, r2, r1					;
	loadi r4, r2					; terceira := tabuleiro[6]
	cmp r3, r4						; se primeira != terceira:
	jne checa_diagonal_2__fim		;     saia
									;
	mov r0, r3 						; vencedor := primeira
									;
checa_diagonal_2__fim:				;
	store vencedor, r0				;
	
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
	
	loadn r0, #' '					; vencedor := NENHUM
	loadn r1, #9					;
									;
	load r2, rodada					;
	cmp r1, r2						; se rodada = 9:
	jne checa_rodada__fim			;
									;
	loadn r0, #'-'					;     vencedor := EMPATE
									;
checa_rodada__fim:					;
	store vencedor, r0				;

	pop r2
	pop r1
	pop r0
	pop fr
	rts


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
	loadn r1, #tabuleiro			; casa := &tabuleiro
	loadn r2, #9					;
	add r2, r2, r1					; última := casa + 9
									;
cria_tabuleiro__loop:				; repita:
	storei r1, r0					;     *casa := ' '
	inc r1							;     casa++
	cmp r1, r2						;
	jle cria_tabuleiro__loop		; enquanto casa < última
	
	pop r2
	pop r1
	pop r0
	pop fr
	rts


preenche_tabuleiro:
	push fr
	push r0
	push r1
	
	loadn r0, #tabuleiro			;
	load r1, posicao				;
	add r0, r0, r1					; casa := &tabuleiro[posição]
									;
	load r1, jogador				;
	storei r0, r1					; *casa = jogador
	
	pop r1
	pop r0
	pop fr
	rts


; Posição

digito_para_posicao:
	push fr
	push r0
	
	load r0, digito					;
	dec r0							;
	store posicao, r0				; posicao := digito - 1
	
	pop r0
	pop fr
	rts


checa_posicao:
	push fr
	push r0
	push r1
	push r2
	
	loadn r0, #0					;
									;
	loadn r1, #tabuleiro			;
	load r2, posicao				;
	add r1, r1, r2					;
	loadi r1, r1					; casa := tabuleiro[posição]
									;
	loadn r2, #' '					; VAZIO = ' '
	cmp r1, r2						; se casa != VAZIO:
	jne checa_posicao__fim			;     posicao_valida := falso
									; senão:
	loadn r0, #1					;     posicao_valida := verdadeiro
	
checa_posicao__fim:
	store posicao_valida, r0
	
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
le_caractere__loop:					; repita:
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
	loadn r2, #'1'					;
	cmp r1, r2						; se caractere < '1':
	jle checa_digito__fim			;     digito_valido := falso
									;
	loadn r2, #'9'					; senão se caractere > '9':
	cmp r1, r2						;     digito_valido := falso
	jgr checa_digito__fim			;
									; senão:
	loadn r0, #1					;     digito_valido := verdadeiro
									;
checa_digito__fim:					;
	store digito_valido, r0			;
	
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
