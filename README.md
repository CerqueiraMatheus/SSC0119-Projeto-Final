# Projeto final de Laboratório de Organização de Computadores

> Projeto de adição da operação ADDN ao Processador ICMC e de implementação de um Jogo da Velha, componente final da avaliação de Prática em Organização de Computadores ([SSC0119](https://uspdigital.usp.br/jupiterweb/obterDisciplina?sgldis=SSC0119)), ministrada pelo professor Eduardo do Valle Simões no ICMC/USP durante o primeiro semestre de 2021.

[Clique aqui para assistir ao vídeo de descrição do projeto](google.com).

## Alterações no Processador ICMC

### Operação ADDN

A operação ``ADDN`` (Add Immediate) consiste na adição entre um registrador e um valor imediato informado. Assim, uma implementação bem-sucedida deve ser capaz de interpretar uma instrução no seguinte formato:

```
addn rx, ry, #NR
```

No exemplo, a operação deve produzir um resultado condizente ao valor contido em ``ry`` adicionado a ``#NR``, um número qualquer informado. Por dividir a instrução, é possível adicionar um valor imediato de 16 bits, 2<sup>16</sup> - 1, ou 65.535.

A descrição completa da instrução está disponível no arquivo ``proc-addn.pdf``.

### Montador

O montador foi atualizado a partir do [código-fonte original](https://github.com/simoesusp/Processador-ICMC/tree/master/NovoMontadorLinux) e seguindo os [passos disponibilizados](https://gitlab.com/simoesusp/disciplinas/-/tree/master/SSC0119-Pratica-em-Organizacao-de-Computadores/Modificar_Montador) para que fosse possível a compreensão da instrução ``ADDN``. Foram adicionados o código da instrução, separadores e quantidades de linhas e a lógica de montagem e escrita nos arquivos ``defs.h`` e ``montador.c``.

O montador alterado está disponibilizado sob a pasta ``/Montador``.

### Simulador

O simulador foi refatorado a partir do [código-fonte original](https://github.com/simoesusp/Processador-ICMC/blob/master/Simple_Simulator/simple_simulator_template.c), melhorando a modularização através da separação de arquivos, encapsulamento usando técnicas de TAD, além da renomeação de funções, constantes e variáveis, tornando mais inteligível e facilitando futuras alterações. Foi adicionado o código da operação ``ADDN`` às constantes, criado um sinal de controle para ``IMEDIATO`` no mux ``M4`` e, então, adicionada lógica para a operação no ciclo de ``DECODIFICA`` do processador. Para realizar a operação na ``ULA``, utilizou-se a lógica já implementada para a operação ``ADD``.

O simulador alterado está disponibilizado sob a pasta ``/Simulador``.

## Jogo

Foi escolhida a implementação de um Jogo da Velha para a segunda parte do projeto. Implementado em Assembly, o jogo permite que 2 jogadores se enfrentem e, a cada rodada, o jogador vencedor recebe um ponto no placar. Para jogar, o usuário deve digitar um número entre 1 e 9, que representa uma determinada casa do tabuleiro virtual.

<p float="left" align="center">
  <img src="/prints/1.png" width="500" />
</p>

<p float="left" align="center">
  <img src="/prints/2.png" width="500" />
</p>

<p float="left" align="center">
  <img src="/prints/3.png" width="500" />
</p>

<p float="left" align="center">
  <img src="/prints/4.png" width="500" />
</p>

O jogo desenvolvido está disponibilizado sob a pasta ``/Jogo``.

## Desenvolvedores

[Gustavo Henrique Brunelli](https://github.com/GBrunelli) (11801053)

[Matheus Henrique de Cerqueira Pinto](https://github.com/CerqueiraMatheus) (11911104)

[Pedro Lucas de Moliner de Castro](https://github.com/pedrolmcastro) (11795784)
