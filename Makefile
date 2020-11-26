# LC - compilador L
SHELL = /bin/sh
.POSIX:

# LC versao
VERSION = 1.0.0

CC=gcc
CPP=g++

#flags
LCCFLAGS=

PROJ_NAME = LC

all: options $(PROJ_NAME) mono

options:
	@echo LC build options:
	@echo "CFLAGS = $(LCCFLAGS)"

$(PROJ_NAME):
	$(CC) -o $@ src/$(PROJ_NAME).c

%.o: %.c %.h
	$(CC) -o $@ $< $(LCCFLAGS)

clean:
	rm -f $(PROJ_NAME) $(PROJ_NAME)-mono 8086/prog.asm prog.asm

mono:
	$(CPP) -o $(PROJ_NAME)-mono monolitico/LC.cpp

run:
	./$(PROJ_NAME) -f examples/fonte.l -o prog.asm
	cp prog.asm 8086/prog.asm

runm:
	./$(PROJ_NAME)-mono < examples/fonte.l

no_debug:
	sed -i "s/#define DEBUG_LEX/\/\/define DEBUG_LEX/g" src/lexan.h
	sed -i "s/#define DEBUG_SIN/\/\/define DEBUG_SIN/g" src/ansin.h
	sed -i "s/#define DEBUG_TS/\/\/define DEBUG_TS/g"   src/ts.h
	sed -i "s/#define DEBUG_GEN/\/\/define DEBUG_GEN/g" src/codegen.h

debug:
	sed -i "s/\/\/define DEBUG_LEX/#define DEBUG_LEX/g" src/lexan.h
	sed -i "s/\/\/define DEBUG_SIN/#define DEBUG_SIN/g" src/ansin.h
	sed -i "s/\/\/define DEBUG_GEN/#define DEBUG_GEN/g" src/codegen.h

debug_full:
	sed -i "s/\/\/define DEBUG_LEX/#define DEBUG_LEX/g" src/lexan.h
	sed -i "s/\/\/define DEBUG_SIN/#define DEBUG_SIN/g" src/ansin.h
	sed -i "s/\/\/define DEBUG_GEN/#define DEBUG_GEN/g" src/codegen.h
	sed -i "s/\/\/define DEBUG_TS/#define DEBUG_TS/g"   src/ts.h

debug_gdb:
	gcc -o LCDEBUG.out src/LC.c -g
	gdb ./LCDEBUG.out

dosbox:
	cd 8086/ && dosbox .

.PHONY: all options clean

