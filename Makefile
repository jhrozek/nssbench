all: nssbench

nssbench: nssbench.c
	$(CC) nssbench.c -ggdb -o nssbench

clean:
	rm -f nssbench
