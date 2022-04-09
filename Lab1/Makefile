CC = gcc
CFLAGS = -Wall -lm -lniceprint -L .
out = lab1
source = Funciones.c lab1.c
outfiles = out1_35 out2_35 out3_30000 out4_1M
outf = $(outfiles:=.txt)
objects = $(source:.c=.o)

$(out): $(objects)
	$(CC) -o $@ $^ $(CFLAGS)

$(objects): %.o:%.c
	$(CC) -c $*.c

clean:
	rm -f $(out) $(objects)
run:
	./$(out) -N 35 -i input.txt -o output.txt
	./$(out) -N 35 -i input.txt -o output.txt -D
fullTest:
	./$(out) -N 35 -i test1_35.txt -o out1_35.txt -D
	./$(out) -N 35 -i test2_35.txt -o out2_35.txt -D
	./$(out) -N 30000 -i test3_30000.txt -o out3_30000.txt -D
	./$(out) -N 1000000 -i test4_1M.txt -o out4_1M.txt -D
fullClean:
	rm -f $(out) $(outf)

