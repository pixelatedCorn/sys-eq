main.exe: main.o
	gcc -o bin/main.exe obj/main.o

main.o:
	gcc -c -o obj/main.o src/main.c