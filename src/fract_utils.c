#include "fract_utils.h"
#include <string.h>

/*Reads grayscale file*/
UCHAR read_bmp_index(struct lab_file * input)
{
    UINT	x, y;
    UCHAR* iter=input->i=(UCHAR*)malloc(input->width*input->height);
    if(iter==NULL)
    {
        perror("Error during malloc of index color table\n");
        return BMP_OUT_OF_MEMORY;
    }
    for ( x = 0 ; x < input->width ; ++x )
    {
        for ( y = 0 ; y < input->height ; ++y )
        {
            BMP_GetPixelIndex( input->bmp, x, y, iter );
            BMP_CHECK_ERROR( stdout, -2 );
            iter++;
        }
    }
    return BMP_OK;
}

UCHAR read_bmp_color(struct lab_file * input)
{
    UINT	x, y,iter=0;
    UCHAR* cols[3];
    for(int i=0; i<3; i++)
    {
        cols[i]=(UCHAR*)malloc(input->width*input->height);
        if(cols[i]==NULL)
        {
            fprintf (stderr, "Error during malloc of color table\n%s\n", strerror (12));
            return BMP_OUT_OF_MEMORY;
        }
    }
    for ( x = 0 ; x < input->width ; ++x )
    {
        for ( y = 0 ; y < input->height ; ++y)
        {
            BMP_GetPixelRGB( input->bmp, x, y, &(cols[0][iter]), &(cols[1][iter]), &(cols[2][iter]));
            BMP_CHECK_ERROR( stdout, -2 );
            iter++;
        }
    }
    input->r=cols[0];
    input->g=cols[1];
    input->b=cols[2];
    return BMP_OK;
}

UCHAR save_bmp_index(struct lab_file * input,const char * output_name)
{
    UINT	x, y;
    UCHAR* iter=input->i;
    for ( x = 0 ; x < input->width ; ++x )
    {
        for ( y = 0 ; y < input->height ; ++y )
        {
            BMP_SetPixelIndex( input->bmp, x, y, *iter);
            BMP_CHECK_ERROR( stdout, -2 );
            iter++;
        }
    }
    BMP_WriteFile( input->bmp, output_name );
    BMP_CHECK_ERROR( stdout, -2 );
    return BMP_OK;
}

UCHAR save_bmp_color(struct lab_file * input,const char * output_name)
{
    UINT	x, y,iter=0;
    for ( x = 0 ; x < input->width ; ++x )
    {
        for ( y = 0 ; y < input->height ; ++y )
        {
            BMP_SetPixelRGB( input->bmp, x, y, input->r[iter], input->g[iter], input->b[iter]);
            BMP_CHECK_ERROR( stdout, -2 );
            iter++;
        }
    }
    BMP_WriteFile( input->bmp, output_name );
    BMP_CHECK_ERROR( stdout, -2 );
    return BMP_OK;
}

UCHAR bmp_full_read(struct lab_file *input,const char * input_name)
{
    BMP * inp_bmp = BMP_ReadFile( input_name );
    BMP_CHECK_ERROR( stdout, -1 );

    input->width = BMP_GetWidth( inp_bmp );
    BMP_CHECK_ERROR( stdout, -1 );
    input->height = BMP_GetHeight( inp_bmp );
    BMP_CHECK_ERROR( stdout, -1 );

    int size=BMP_GetDepth( inp_bmp );
    switch(size)
    {
    case 24:
        input->is_index=0;
        break;
    case 8:
        input->is_index=1;
        break;
    default:
        fprintf(stderr,"Read file of unknown depth (not uncompressed 24 or 8 bit bmp)");
        exit(EXIT_FAILURE);
    }

    BMP_CHECK_ERROR( stdout, -1 );
    input->bmp=inp_bmp;
    return 0;
}

void save_bmp(struct lab_file *input,const char * output_name)  //save BMP to file
{
    if(input->is_index)
    {
        if(save_bmp_index(input,output_name))
        {
            fprintf(stderr, "Error during bmp index saving\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        if(save_bmp_color(input,output_name))
        {
            fprintf(stderr, "Error during bmp color saving\n");
            exit(EXIT_FAILURE);
        }
    }
}

UCHAR read_bmp(const char * input_name, struct lab_file * file_meta)
{
    struct lab_file input;

    if(bmp_full_read(&input,input_name))
    {
        fprintf(stderr, "Error during bmp header file reading\n");
        return 1;
    }

    if(input.is_index)
    {
        if(read_bmp_index(&input))
        {
            fprintf(stderr, "Error during bmp index table reading\n");
            return 2;
        }
    }
    else
    {
        if(read_bmp_color(&input))
        {
            fprintf(stderr, "Error during bmp color tables reading\n");
            return 3;
        }
    }

    *file_meta=input;
    return 0;
}

void make_fit(double * tabx,UCHAR * taby,UINT size,UINT jump_size,struct fit * meta)
{
    //size - rozmiar bloku typowo w 1D
    //jump_size = rozmiar tablicy taby - jej szerokosc
    double sx, sxx,sy, sxy;
    sx=sxx=sy=sxy=0;

    jump_size=jump_size-size;

    UINT size_sq=size*size;

    for(int y=0,iter=0,jump=0; y<size; y++)
    {
        for(int x=0; x<size; x++,iter++,jump++)
        {
            sx+=tabx[iter];
            sy+=taby[jump];
            sxx+=tabx[iter]*tabx[iter];
            sxy+=tabx[iter]*taby[jump];
        }
        jump+=jump_size;
    }

    double a=(size_sq*sxy-sx*sy)/(size_sq*sxx-sx*sx);
    double b=(sy-a*sx)/(size_sq);
    double err=0;

    for(int y=0,iter=0,jump=0; y<size; y++)
    {
        for(int x=0; x<size; x++,iter++,jump++)
        {
            double v=taby[jump]-a*tabx[iter]-b;
            err+=v*v;
        }
        jump+=jump_size;
    }
    meta->a=a;
    meta->b=b;
    meta->err=err/size_sq;
}
