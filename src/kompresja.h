#ifndef KOMPRESJA_H_INCLUDED
#define KOMPRESJA_H_INCLUDED
#include <stdlib.h>

/*Struktury danych*/
struct zapamietane
{
	double a;
	double b;
    UINT wskx;
};

/*Prototypy funkcji*/
void kompresja (struct lab_file *input, int kolor, struct zapamietane *output, struct zapamietane *outputcol, int quick);
double* zamiana16na8 (struct lab_file *input, UINT index16, int RGB, double t8x8[64]);
void dekompresja (struct lab_file *input, int kolor, struct zapamietane *output, struct zapamietane *outputcol);


#endif /* KOMPRESJA_H_INCLUDED */








