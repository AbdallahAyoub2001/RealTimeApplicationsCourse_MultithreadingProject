CC = gcc -g
O = -o

files:
	$(CC) $(O) main main.c -lpthread -lm

run: main
	./main arguments.txt

clean:
	rm -f *.o main