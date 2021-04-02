build:
	gcc -Wall -g Tema1.c -o editor

run:
	./editor < editor.in

valgrind:
	valgrind --tool=memcheck --leak-check=full ./editor < editor.in

clean:
	rm -rf ./editor
