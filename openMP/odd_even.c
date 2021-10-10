#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
const int RMAX = 100000;
int thread_count;
void gen(int a[], int n) {
   int i;
   srandom(1);
   for (i = 0; i < n; i++)
      a[i] = random() % RMAX;
} 

void Print(int a[], int n) {
   int i;
   for (i = 0; i < n; i++)
      printf("%d ", a[i]);
   printf("\n\n");
}  
void Odd_even(int a[], int n) {
   int phase, i, tmp;
#  pragma omp parallel num_threads(thread_count) \ 
        default(none) shared(a, n) private(i, tmp, phase)
   for (phase = 0; phase < n; phase++) {
      if (phase % 2 == 0)
#        pragma omp for 
         for (i = 1; i < n; i += 2) {
            if (a[i-1] > a[i]) {
               tmp = a[i-1];
               a[i-1] = a[i];
               a[i] = tmp;
            }
         }
      else
#        pragma omp for 
         for (i = 1; i < n-1; i += 2) {
            if (a[i] > a[i+1]) {
               tmp = a[i+1];
               a[i+1] = a[i];
               a[i] = tmp;
            }
         }
   }
}  /* Odd_even */

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    int  n=1000;
    char g_i;
    int* a;
    thread_count = atoi(argv[1]); 
    a = malloc(n*sizeof(int));
    gen(a,n);
    Print(a,n);
    double time_start= omp_get_wtime();
    Odd_even(a, n);
    double time_end= omp_get_wtime();
    Print(a,n);
    printf("the time = %e seconds \n",  time_end - time_start);
}