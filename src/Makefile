CC=tigcc
CCFLAGS=-O2 -std=c99

all: tigol
clean:
	rm -rf tigol tigol.o game.o tigol.89z tigol.9xz tigol.v2z

tigol: tigol.o game.o
	$(CC) $(CCFLAGS) -o tigol tigol.o game.o

tigol.o: tigol.c
	$(CC) $(CCFLAGS) -c tigol.c

game.o: game.c
	$(CC) $(CCFLAGS) -c game.c
