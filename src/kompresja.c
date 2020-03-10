#include "fract_utils.h"
#include "kompresja.h"

void kompresja(struct lab_file *input, int kolor, struct zapamietane *output, struct zapamietane *outputcol, int quick)
{
	/*Zmienne funkcji makefit*/
	double *tabx=0; //wskaźnik na pierwszy piksel uśrednionego 16x16 do 8x8
    UCHAR *taby = NULL; //wskaźnik na pierwszy piksel każdego 8x8
	int size=8; //rozmiar fraktala
	struct fit meta;

	/*Pozostałe zmienne*/
	UINT dozapamietania=0;
	double zapamietaja=0;
	double zapamietajb=0;
	double t8x8[64]={0}; //16x16 uśredniona do 8x8
	int postep=0;

	/*Kompresja kolorowa*/
	if (kolor == 1)
	{
        postep=0;
		printf ("Preparing picture in color...\n\n");
		for (int RGB=0;RGB<3;RGB++)
		{
			/*8x8*/
			printf ("Left: %lu\n", 3*(input->height*input->width/64));
			for (int k=0;k<input->width/8;k++) //kolumny - 32 razy dla 256x256
			{
				for (int w=0;w<input->height/8;w++) //wiersze - 32 razy dla 256x256
				{
					UINT indeks8 = (8*k*input->width)+(w*8); //wskaźnik na róg 8x8

					/*taby - wskaźnik na 8x8 wysyłany do makefit func*/
					if (RGB == 0) taby = (input->r+indeks8);
					else if (RGB == 1) taby = (input->g+indeks8);
					else if (RGB == 2) taby = (input->b+indeks8);

					/*16x16*/
					double errmin = 10000;
                    UINT petla=0;
                    if (quick == 1) petla = 50;
                    else if (quick == 0) petla = (input->width-15);
					for (int dk=0;dk<petla;dk++) //kolumny - 241 razy dla 256x256
					{
						for (int dw=0;dw<petla;dw++) //wiersze - 241 razy dla 256x256
						{
							//UINT indeks16 = dw + (dk*input->width); //wskaźnik na róg 16x16
                            UINT indeks16 = rand()%((input->width-15)*(input->height-15));

							zamiana16na8(input, indeks16, RGB, t8x8); //Uśrdnianie 16x16 do 8x8
							tabx = &t8x8[0]; //tabx - wskaźnik na pierwszy element uśrednionej 16x16

							make_fit(tabx, taby, size, input->width, &meta); //makefit func
							if (meta.err < errmin)
							{
								dozapamietania = indeks16;
								zapamietaja = meta.a;
								zapamietajb = meta.b;
								errmin = meta.err;
							}
						}
					}
                    /*zapamiętywanie a, b i wskx*/
                    outputcol[postep].a = zapamietaja;
                    outputcol[postep].b = zapamietajb;
                    outputcol[postep].wskx = dozapamietania;
                    //printf("%d\t %f\t %f\t %lu\t %f\n", postep, zapamietaja, zapamietajb, dozapamietania, errmin);
					postep++;
					printf ("Left: %lu\n", 3*(input->height*input->width/64)-postep);
				}
			}
		}
		printf ("Done.\n");
	}

	/*Kompresja czarno-biała*/
	else if (kolor == 0)
	{
        postep=0;
		printf ("Preparing Black&White Picture...\n\n");
		int RGB=-1;

		/*8x8*/
		printf ("Left: %lu\n", (input->height*input->width/64));
		for (int k=0;k<input->width/8;k++) //kolumny - 32 razy dla 256x256
		{
			for (int w=0;w<input->height/8;w++) //wiersze - 32 razy dla 256x256
			{
				UINT indeks8 = (8*k*input->width)+(w*8); //wskaźnik na róg 8x8
				taby = input->i+indeks8; //taby - wskaźnik na 8x8 wysyłany do makefit func

				/*16x16*/
				double errmin = 10000;
                UINT petla=0;
                if (quick == 1) petla = 50;
                else if (quick == 0) petla = (input->width-15);
				for (int dk=0;dk<petla;dk++) //kolumny - 241 razy dla 256x256
				{
					for (int dw=0;dw<petla;dw++) //wiersze - 241 razy dla 256x256
					{
						//UINT indeks16 = dw + (dk*input->width); //wskaźnik na róg 16x16
                        UINT indeks16 = rand()%((input->width-15)*(input->height-15));

						zamiana16na8(input, indeks16, RGB, t8x8); //Uśrdnianie 16x16 do 8x8
						tabx = &t8x8[0]; //tabx - wskaźnik na pierwszy element uśrednionej 16x16

						make_fit(tabx, taby, size, input->width, &meta); // makefit func
						if (meta.err < errmin)
						{
							dozapamietania = indeks16;
							zapamietaja = meta.a;
							zapamietajb = meta.b;
							errmin = meta.err;
						}
					}
				}
                /*zapamiętywanie a, b i wskx*/
                output[postep].a = zapamietaja;
                output[postep].b = zapamietajb;
                output[postep].wskx = dozapamietania;
                //printf("%d\t %f\t %f\t %lu\t %f\n", postep, zapamietaja, zapamietajb, dozapamietania, errmin);
				postep++;
				printf ("Left: %lu\n", (input->height*input->width/64)-postep);
			}
		}
		printf ("Done.\n");
	}
}

double* zamiana16na8 (struct lab_file *input, UINT index16, int RGB, double t8x8[64])
{
	/*Zmienne*/
	int i8x8=0; //iterowanie tablicy 8x8
	UINT jump2k=0; //skok o 2 kolumny

	/*Uśrednianie*/
	for (int n=0;n<8;n++)
	{
		for (int m=0;m<8;m++)
		{
			UINT i16 = index16 + 2*m + jump2k;

			if (RGB==0) t8x8[i8x8] =
					((*(input->r+i16))+(*(input->r+i16+1))+(*(input->r+i16+input->width))+(*(input->r+i16+input->width+1))) / 4.0;
			else if (RGB==1) t8x8[i8x8] =
					( (*(input->g+i16))+(*(input->g+i16+1))+(*(input->g+i16+input->width))+(*(input->g+i16+input->width+1)) ) / 4.0;
			else if (RGB==2) t8x8[i8x8] =
					( (*(input->b+i16))+(*(input->b+i16+1))+(*(input->b+i16+input->width))+(*(input->b+i16+input->width+1)) ) / 4.0;
			else if (RGB==-1) t8x8[i8x8] =
					( (*(input->i+i16))+(*(input->i+i16+1))+(*(input->i+i16+input->width))+(*(input->i+i16+input->width+1)) ) / 4.0;
			i8x8++;
		}
		jump2k = jump2k + 2*input->width; //skok o 2 kolumny
	}
	return t8x8;
}

void dekompresja (struct lab_file *input, int kolor, struct zapamietane *output, struct zapamietane *outputcol)
{
	/*Zmienne*/
	double t8x8[64]={0}; //16x16 uśredniona do 8x8
    UINT indeks16 = 0;

	/*Tworzenie "szumu"*/
	for (int i=0;i<input->width*input->height;i++)
	{
		if (kolor==1)
		{
			(*(input->r+i)) = rand()%256;
			(*(input->g+i)) = rand()%256;
			(*(input->b+i)) = rand()%256;
		}
		else if (kolor==0)
            (*(input->i+i)) = rand()%256;
	}

    /*Dekompresja czarno-biała*/
    if(kolor == 0)
    {
        for (int ile=0;ile<100;ile++) //wielokrotne powtórzenie operacji
        {
            printf("Left: %d\n", 100-ile);
            int iter=0;
            
			/*8x8*/
			for (int k=0;k<input->width/8;k++) //kolumny - 32 razy dla 256x256
			{
				for (int w=0;w<input->height/8;w++) //wiersze - 32 razy dla 256x256
				{
                    //printf("%d\t %f\t %f\t %lu\n", iter, output[iter].a, output[iter].b, output[iter].wskx);
					UINT indeks = (8*k*input->width)+(w*8);
					UINT jumpk=0; //skok o kolumnę
                    
                    indeks16 = output[iter].wskx; //odczyt zapisanego wskaźnika na 16x16 dla danej ósemki
                    int RGB=-1; //oznaczenie czarno-białej bitmapy
                    zamiana16na8(input, indeks16, RGB, t8x8); //Uśrdnianie 16x16 do 8x8

					int i8x8=0; //iterowanie tablicy 8x8
					for (int n=0;n<8;n++)
					{
						for (int m=0;m<8;m++)
						{
							UINT i8 = indeks + m + jumpk;
                            double a = t8x8[i8x8] * output[iter].a + output[iter].b;
                            if (a>255) a=255;
                            else if (a<0) a=0;
                            
							(*(input->i+i8)) = (UINT)a;
							i8x8++;
						}
						jumpk = jumpk + input->width; //skok o kolumnę
					}
					//printf("%d. %f\t %f\t %d\n", output[iter].data, output[iter].a, output[iter].b, output[iter].wskx);
					iter++;
				}
			}
		}
        printf ("Done.\n");
	}
    
    /*Dekompresja kolorowa*/
    else if (kolor == 1)
    {
        for (int ile=0;ile<100;ile++) //wielokrotne powtórzenie operacji
        {
            printf("Left: %d\n", 100-ile);
            int iter = 0;
            for (int RGB=0;RGB<3;RGB++)
            {
                /*8x8*/
                for (int k=0;k<input->width/8;k++) //kolumny - 32 razy dla 256x256
                {
                    for (int w=0;w<input->height/8;w++) //wiersze - 32 razy dla 256x256
                    {
                        UINT indeks8 = (8*k*input->width)+(w*8);
                        UINT jumpk=0; //skok o kolumnę
                        
                        indeks16 = outputcol[iter].wskx; //odczyt zapisanego wskaźnika na 16x16 dla danej ósemki
                        zamiana16na8(input, indeks16, RGB, t8x8); //Uśrdnianie 16x16 do 8x8
                        
                        int i8x8=0; //iterowanie tablicy 8x8
                        for (int n=0;n<8;n++)
                        {
                            for (int m=0;m<8;m++)
                            {
                                UINT i8 = indeks8 + m + jumpk;
                                double a = t8x8[i8x8] * outputcol[iter].a + outputcol[iter].b;
                                if (a>255) a=255;
                                else if (a<0) a=0;
                                
                                if (RGB==0) (*(input->r+i8)) = (UINT)a;
                                else if (RGB==1) (*(input->g+i8)) = (UINT)a;
                                else if (RGB==2) (*(input->b+i8)) = (UINT)a;
                                i8x8++;
                            }
                            jumpk = jumpk + input->width; //skok o kolumnę
                        }
                        iter++;
                    }
                }
            }
        }
    }
    printf ("Done.\n");
}
