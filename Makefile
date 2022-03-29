wish: main.o wordFormatting.o built-in.o
	gcc -o wish main.o wordFormatting.o built-in.o

main.o: main.c
	gcc -c main.c

wordFormatting.o : wordFormatting.c
	gcc -c wordFormatting.c

built-in.o : built-in.c
	gcc -c built-in.c
