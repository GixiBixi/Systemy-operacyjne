CC     = gcc
CFLAGS = -Wall -std=c99 -pedantic
OBJS   = main.o cwiczenie3a.o cwiczenie3b.o cwiczenie3c.o

all: ex3a ex3b1 ex3b2 ex3c1 ex3c2 ex3c3

ex3a ex3b1 ex3b2 ex3c1 ex3c2 ex3c3: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) ex3a ex3b1 ex3b2 ex3c1 ex3c2 ex3c3