#ifndef FRACT_UTILS_H_INCLUDED
#define FRACT_UTILS_H_INCLUDED

#include <stdlib.h>
#include "qdbmp.h"

struct lab_file     // wczytany obraz, calosc
{
    UINT width;
    UINT height;
    UCHAR * r;          // if color
    UCHAR * g;          // if color
    UCHAR * b;          // if color
    UCHAR * i;          // if color
    UCHAR is_index;     // if black&white
    BMP * bmp;
};

struct fit
{
    double a;
    double b;
    double err;
    UINT x;
    UINT y;
};

void save_bmp(struct lab_file *input,const char * output_name);
UCHAR read_bmp(const char * input_name, struct lab_file * file_meta);
void make_fit(double * tabx,UCHAR * taby,UINT size,UINT jump_size,struct fit *meta);

#endif // FRACT_UTILS_H_INCLUDED
