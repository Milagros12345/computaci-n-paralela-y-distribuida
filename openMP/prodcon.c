#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
/*
compile:gcc -fopenmp -o prodcon prodcon.c
execute: ./prodcon
*/
#define BUFFER_SIZE 5
#define NITER 10
void *producer();
void *consumer();

int buffer[BUFFER_SIZE];
int in = 0;   
int out = 0; 
int main(){
   srand(time(NULL));
   #pragma omp parallel sections
   {
      #pragma omp section
      producer();
      
      #pragma omp section
      consumer();
   }
   return 0;
}
void *producer(){
   int next_produced; 
   for (int i=0; i< NITER; i++) { 
      next_produced=rand() % 100 + 1;
      while (((in + 1) % BUFFER_SIZE) == out) 
         ; 
      buffer[in] = next_produced; 
      printf("Productor produce [%d].(para index:in=%d,out=%d)\n",next_produced,in,out);     
      in = (in + 1) % BUFFER_SIZE; 
   }
}

void *consumer(){
   int next_consumed; 
   for (int i=0; i< NITER; i++) { 
      while (in == out); 
      next_consumed = buffer[out]; 
      printf("\t\tConsumidor consume [%d].(in=%d,Consume de index: out=%d)\n",next_consumed,in,out);     
      out = (out + 1) % BUFFER_SIZE;

   } 
}