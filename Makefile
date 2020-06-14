CC = gcc

CFLAGS = -o -Wall

SOURCES := $(wildcard *.c)

clean:
	rm argus argusd
	rm -r *.o
	@echo Objetos e Executável Apagados

cleanall: cleanech
	@echo Documentação Apagada

argus : listar.o executar.o argus.o
				$(CC) $(CFLAGS) argus listar.o executar.o argus.o

argusd : listar.o executar.o argusd.o
				$(CC) $(CFLAGS) argus listar.o executar.o argus.o

listar.o : listar.c listar.h
executar.o : executar.c executar.h
argus.o : argus.c argus.h listar.h listar.h
argusd.o : argusd.c argus.h listar.h listar.h
