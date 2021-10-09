#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <chrono>
using namespace std;
/**********************************************************************/
int thread_count;
int m, n;
double *A;
double *x;
double *y;
/*****************************************************************/
//function
/**************************************************************/
void *Pth_mat_vect(void * rank) {
    long my_rank = (long)rank;
    int i,j;
    int local_m = m/thread_count;
    int my_first_row = my_rank*local_m;
    int my_last_row = (my_rank+1)*local_m-1;
    for (i = my_first_row; i <= my_last_row; i++) {
        y[i] = 0.0;
        for (j = 0; j < n; j++)
            y[i] += A[i*n+j]*x[j];
    }
    return NULL;
} /* Pth mat vect */
void gen(double *matrix, int n_row, int n_col){
	for (int i = 0; i < n_row; i++){//filas
		for (int j = 0; j < n_col; j++){//columnas
            matrix[i*n_col+j]=rand()%20;
		}  
	}  
}  
void Print( double *vec,double *A, int m, int n) {
   for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++)
        cout<< A[i*n+j]<<" ";
    cout<<endl;
   }
   for(int i=0;i<m;i++){
       cout<<vec[i]<<endl;
   }
} 

int main(int argc, char* argv[]) {
    pthread_t* threads;

    cout<<" Ingrese la cantidad de threads "<<endl;
    cin>>thread_count;
    cout<<"Ingrese el m(fila) y n(columna) para mxn "<<endl;
    cin>>m>>n;
    threads= new pthread_t[thread_count];
    A = new double[m*n];
    x = new double[n];
    y = new double[m];
    gen(A,m,n);
    gen(x,m,1);
    Print(x,A,m,n);
    auto start_time= chrono::high_resolution_clock::now();
    for(long i = 0; i< thread_count; i++){
        pthread_create(&threads[i], NULL,Pth_mat_vect, (void*)i);
    }
   for(long i = 0; i< thread_count; i++){
        pthread_join(threads[i], NULL);
    }
    Print(y,A,m,n);
    auto end_time=chrono::high_resolution_clock::now();
    cout<<"time: "<<chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count()<<endl;
    free(A);
    free(x);
    free(y);
    free(threads);
    return 0;
}