# Jogo
O jogo implementado em assembly para o processador ICMC foi o jogo da velha. O jogo permite que 2 jogadores se enfrentem, e a cada rodada o jogador vencedor recebe um ponto no placar. Para jogar, o usuário deve digitar um número entre 1 e 9, que representam as casas do  tabuleiro virtual.

# Simulador
O simulador foi refatorado a partir do [código original](https://github.com/simoesusp/Processador-ICMC/blob/master/Simple_Simulator/simple_simulator_template.c), melhorando a modularização, e o encapsulamento usando técnicas de TAD, a instrução escolhida para ser adicionada à arquitetura foi a adicionar um valor imediato. Para utilizar a instrução, usasse o seguinte comando addn, segue um exemplo de como somar o valor inteiro 2 com o conteúdo do registrador r2, e colocar o resultado em r1: 
```
addn r1, r2, #2
```
A instrução, depois que passa pelo montador, é divida em 2 linhas, a primeira contém os bits de opcode, ambos os registradores, e o valor de carry, a segunda linha é usada exclusivamente para o valor imediato. Por dividir a instrução, é possível adicionar um valor imediato de 16 bits, 2<sup>16</sup> - 1, ou 65.535.


### Desenvolvedores:
Gustavo Brunelli - Nº USP 11801053
Matheus Cerqueira - Nº USP 11911104
Pedro Castro - Nº USP 11795784


