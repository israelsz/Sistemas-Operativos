CC = gcc
CFLAGS = -Wall -lm -lpthread -lniceprint -L .
out = lab3
source = Funciones.c lab3.c 
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
	./$(out) -N 35 -c 5 -i test1_35.txt -o output.txt -b 3 -h 5 -D
	./$(out) -N 1000000 -c 5000  -i test4_1M.txt -o output.txt -b 1000 -h 53 -D
fullTest:
	./$(out) -N 35 -c 5 -i test1_35.txt -o out1_35.txt -b 5 -h 3  -D
	./$(out) -N 35 -c 7 -i test2_35.txt -o out2_35.txt -b 3 -h 2 -D
	./$(out) -N 30000 -c 20000 -i test3_30000.txt -o out3_30000.txt -b 800 -h 125 -D
	./$(out) -N 1000000 -c 5000 -i test4_1M.txt -o out4_1M.txt -b 1500 -h 60 -D
fullClean:
	rm -f $(out) $(outf)

