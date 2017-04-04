# part of the makefile

RM = rm
CFLAGS = -O0 -g
OBJECT = main.c http.c

all: $(OBJECT)
	gcc -o web $(CFLAGS) $(OBJECT) -l pthread

clean:
	$(RM) *.o *.exe
