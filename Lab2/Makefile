CC = gcc
CFLAGS = -Wall -lm -lniceprint -L .
outLab = lab2
outBomb = bomb
depLab = Funciones.o lab2.o
depCal = Funciones.o bomb.o
source = Funciones.c lab2.c bomb.c
salidas=$(outLab) $(outBomb)
objects = $(source:.c=.o)
all: $(salidas)
$(outBomb): $(depCal)
	$(CC) -o $@ $^ $(CFLAGS)
$(outLab): $(depLab)
	$(CC) -o $@ $^ $(CFLAGS)

$(objects): %.o:%.c
	$(CC) -c $*.c

run:
	./lab2 -N 35 -p 2 -c 5 -i test1_35.txt -o output.txt -D
clean:
	rm -f $(outBomb) $(outLab) $(objects)