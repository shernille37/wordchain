CC := gcc

solution: main.o file.o dict.o states.o utils.o
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $< -o $@
	
.PHONY: clean

clean:
	rm -f *.o