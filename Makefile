LIB_NAME='CHANGE_ME'

all:
	gcc -O2 -Wall -Werror -g -c -fPIC lib.c -o lib.o
	ar rcs lib$(LIB_NAME).a lib.o
	gcc -shared -Wl,-soname,lib.o -lm -o lib$(LIB_NAME).so \
	 lib.o
	#make -C tests/

test:
	make -C tests/

clean:
	rm -f *.so
	rm -f *.o
	rm -f *.a
	rm -f tests/test*

