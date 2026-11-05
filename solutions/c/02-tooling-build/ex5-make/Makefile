CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Werror -g

# "all" is the default goal: the first target in the file.
all: demo test_mathx

demo: main.o mathx.o
	$(CC) $(CFLAGS) main.o mathx.o -o demo

test_mathx: test_mathx.o mathx.o
	$(CC) $(CFLAGS) test_mathx.o mathx.o -o test_mathx

# Pattern rule: any .o is built from the .c with the same name.
# Every object also depends on the header, so editing mathx.h rebuilds them.
%.o: %.c mathx.h
	$(CC) $(CFLAGS) -c $< -o $@

test: test_mathx
	./test_mathx

clean:
	rm -f *.o demo test_mathx demo.exe test_mathx.exe

.PHONY: all test clean
