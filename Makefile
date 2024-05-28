CC := gcc
GSL_FLAGS := $(shell gsl-config --cflags)
GSL_LIBS := $(shell gsl-config --libs)

solution: main.o file.o dict.o states.o utils.o multiUtils.o multiProcess.o
	$(CC) $(GSL_FLAGS) -o $@ $^ $(GSL_LIBS)

%.o : %.c
	$(CC) $(GSL_FLAGS) -c $< -o $@
	
.PHONY: clean

clean:
	rm -f *.o