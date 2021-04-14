build:
	gcc -Wall -g -std=c99 Tema1.c stack.c stack.h list.h list.c -o editor

run:
	./editor < editor.in

valgrind:
	valgrind --tool=memcheck --leak-check=full ./editor < editor.in

clean:
	rm -rf ./editor
