all: test_misc.bin

OPTS=
LDOPTS=
CC=/home/fjohnber/gcc.git/_install/bin/gcc
test_misc.bin: test_misc.o libtest.so
	$(CC) $(OPTS) test_misc.o libtest.so -o test_misc.bin $(LDOPTS)

libtest.so: test_lib.o
	$(CC) $(OPTS) -shared -Wl,-soname,libtest.so -o libtest.so test_lib.o

test_lib.o: test_lib.c
	$(CC) $(OPTS) -fPIC -o test_lib.o -c test_lib.c

.PHONY: clean
clean:
	rm -rf test_misc.bin
	rm -rf libtest.so
	rm -rf *.o
	
