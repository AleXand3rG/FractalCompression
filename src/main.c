/*
 * main.c
 *
 *  Created on: Jan 3, 2019
 *      Author: Aleksander Grzybowski
 */

 //============================================================================

    /*
     Aby użyć, wpisz ścieżki w odpowiednie miejsca poniżej.
     */

 //============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "fract_utils.h"
#include "kompresja.h"

int main(int argc, char* argv[])
{
	srand((unsigned int)time(NULL));
	time_t start, end, duration;

	for (;;)
	{
		struct lab_file input;
        
        //============================================================================
        
        /** Wpisz ścieżkę do pliku, który chcesz skompresować */
        read_bmp(
                 "/Users/Olek/Documents/Git/FractalCompression/resources/lena_256.bmp"
                 , &input); //wczytanie bitmapy z pliku
        
        //============================================================================

		/*Zmienne*/
		int kolor=-1; //kolorowe czy czarnobiałe
		int wybor; //menu główne
		int KczyD=0; //kompresja czy dekompresja
        int quick=-1; //szybka kompresja
        
		struct zapamietane outputcol[196608]; //dane po skompresowaniu
		struct zapamietane output[65536]; //dane po skompresowaniu

		/*Sprawdzenie koloru: 1-kolorowy, 0-czarno-biały*/
		if (input.is_index == 0) kolor = 1;
		else if (input.is_index == 1) kolor = 0;

		/*Interfejs*/
		printf("\nWhat would you like to do?\n"
    		"1 - Compression (best quality)\n"
            "2 - Quick Compression\n"
    		"3 - Decompression\n"
			"0 - Exit\n");
		while(scanf("%d", &wybor) != 1)
		{
			int c;
			while((c = getchar()) != '\n' && c != EOF); //czyszczenie bufora
		}
		switch(wybor)
		{
            case 1: KczyD = 1; quick = 0; break; //kompresja
            case 2: KczyD = 2; quick = 1; break; //szybka kompresja
			case 3: KczyD = 3; break; //dekompresja
			case 4: KczyD = 4; break; //full
			case 0: exit(-1); break;
			default: printf("Wrong option.\n\n"); break;
		}

		/*Działanie*/
		if (KczyD == 1) //kompresja
		{
			printf ("Compression starts...\n");
			start = time (0); //start zegara

			kompresja (&input, kolor, output, outputcol, quick);

			end = time (0); //stop zegara
			duration = end - start; //obliczanie czasu
			printf ("Time: %ld seconds.\n", duration);
		}
        
        if (KczyD == 2) //szybka kompresja
        {
            printf ("Quick Compression starts...\n");
            start = time (0); //start zegara
            
            kompresja (&input, kolor, output, outputcol, quick);
            
            end = time (0); //stop zegara
            duration = end - start; //obliczanie czasu
            printf ("Time: %ld seconds.\n", duration);
        }

		else if (KczyD == 3) //dekompresja
		{
			/*Sprawdzenie warunku dekompresji*/
			if (output[0].wskx == 0 && outputcol[0].wskx == 0)
			{
				fprintf (stderr, "Can't find decompression data.\n%s\n", strerror (5));
			}
			else //gdy spełnion
			{
				printf ("Decompression starts...\n");
				start = time (0); //start zegara

				dekompresja(&input, kolor, output, outputcol);
                
                //============================================================================
                       
                /** Wpisz ścieżkę, w której chcesz zapisać zdekompresowany obraz*/
                save_bmp(&input,
                         "output_path/output_filename.bmp"
                         ); //zapis bitmapy do pliku
                       
                //============================================================================
				

				end = time (0); //stop zegara
				duration = end - start; //obliczanie czasu
				printf ("Time: %ld seconds.\n", duration);
			}
		}

		BMP_Free(input.bmp); //zwolnienie pamięci zajmowanej przez bitmapę
	}
	return 0;
}
