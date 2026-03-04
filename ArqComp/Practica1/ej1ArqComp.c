#include "counterH.c"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    double ck;
    start_counter();

    // Poñer aquí o código a medir
    for(int i=0; i<100000000; i++);
    

    ck=get_counter();

    printf("\n Clocks=%1.10lf \n",ck);

   // Esta rutina imprime a frecuencia de reloxo estimada coas rutinas start_counter/get_counter
    mhz(1,1);
    return 0;
}