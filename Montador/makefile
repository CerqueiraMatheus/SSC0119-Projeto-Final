#makefile do montador
COMPILER = gcc
FILES =  montador.c parser.c structs.c
OBJS = $(FILES:.c=.o)
NAME_FLAG = -o
COMP_FLAG = -march=native -03
NAME = montador

all: main.o $(OBJS)
	$(COMPILER) $^ $(NAME_FLAG) $(NAME)

%.o: %.c %.h defs.h
	$(COMPILER) -c $< $(COMP_FLAGS) $(NAME_FLAG) $@

main.o: main.c $(wildcard *.h)
	$(COMPILER) -c $< $(COMP_FLAGS) $(NAME_FLAG) $@

clear:
	rm *.o
	rm $(NAME)
