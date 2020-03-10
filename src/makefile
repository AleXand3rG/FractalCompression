all:	FractalCompression.exe
clean:
	rm main.o fract_utils.o kompresja.o qdbmp.o FractalCompression
FractalCompression.exe: main.o 
	gcc -o FractalCompression main.o kompresja.o fract_utils.o qdbmp.o
main.o:
	gcc main.c -o main.o -c
	gcc kompresja.c -o kompresja.o -c
	gcc fract_utils.c -o fract_utils.o -c
	gcc qdbmp.c -o qdbmp.o -c