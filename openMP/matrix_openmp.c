#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
int RMAX=10;
void gen(int **mat,int *vec, int r,int c){
    int i,j;
    srandom(1);
    printf("Our Matrix is = \n");
	for (i = 0; i < r; i++)
	{
		for (j = 0; j < c; j++)
		{
			mat[i][j] = random() % RMAX;;
			printf("%4d\t", mat[i][j]);
		}
		printf("\n");
	}	

    printf("Our Vector is = \n");
	for (i = 0; i < c; i++)
	{
		vec[i] = random() % RMAX;;
		printf("%4d\n",vec[i]);
	}
}
void mult(int r,int c,int **mat,int *vec, int *res){
    int i,j;
    /* PARALLEL  */
	#pragma omp parallel shared(r,c,mat,vec,res) private(i,j)
	{
		#pragma omp for
		for ( i = 0; i < r; i++ ) 
		{
			res[i] = 0;
			for ( j = 0; j < c; j++ )
			{
				res[i] += mat[i][j]*vec[j];
			}

			printf("Thread %d executes i=%d\n",omp_get_thread_num(),i);
		}
	} /* PARALLEL ENDS */
}
void print(int *res, int r){
    int i;
    printf("Our Result Vector is = \n");
	for (i = 0; i < r; i++)
    {
	    printf("%4d\n",res[i]);
 	}
}
int main(int argc, char *argv[])
{
	double time_start, time_end;
    time_start = omp_get_wtime();
    //matrix[m][n] y vec[n], res[m]
	int m = atoi(argv[1]); 
	int n = atoi(argv[2]);
    int **matrix = (int **)malloc(m * sizeof(int *)); //matrix
    for (int i=0; i<m; i++) 
         matrix[i] = (int *)malloc(n * sizeof(int)); 

    int *vec = (int *) malloc(n * sizeof(int)); //vector
	int *res = (int *) malloc(m * sizeof(int)); //Result vector
    gen(matrix,vec,m,n);
    mult(m,n,matrix,vec,res);
 	time_end = omp_get_wtime();
     print(res,m);
 	printf("\nTime to execute = %e seconds.\n", time_end-time_start);

 	return 0;
}