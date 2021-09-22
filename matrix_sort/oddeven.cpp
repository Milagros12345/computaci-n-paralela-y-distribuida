#include <iostream>
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <algorithm> 
#include <iomanip>
#define N 400000
/*compiler: mpicxx -o oddeven oddeven.cpp
  execution:   mpirun -np 4 ./oddeven
*/
using namespace std;
int compare (const void * a, const void * b){
  return ( *(int*)a > *(int*)b );
}
int main(int argc, char *argv[]){
	int nump,rank;//número de procesos y el rank
	int n,localn;
	int *data,recdata[N],recdata2[N];
	int *temp;
	int i;
	int sendcounts;
	MPI_Status status;
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nump);
    if(rank == 0) {
        cout<<"Ingrese la cantidad de elementos para ordenar: "<<endl;
        cin>>n;
		//N=n;
        int avgn = n / nump;//la cantidad de elemento que le va a tocar a cada proceso
        localn=avgn;
    	data=new int[n];
        for(i = 0; i < n; i++) {
            data[i] = rand()%2000;
        }
        /*cout<<" array generado"<<endl;
         for(i=0;i<n;i++){
         	cout<<data[i]<<" ";
        }
        cout<<endl;*/
    }
    else{
    	data=NULL;
    }
	double timer=MPI_Wtime();
    MPI_Bcast(&localn,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Scatter(data, localn, MPI_INT, &recdata, N, MPI_INT, 0, MPI_COMM_WORLD);
    /*cout<<"el proceso "<<rank<<" recibio datos: ";
         for(i=0;i<localn;i++){
         	cout<<recdata[i]<<" ";
         }*/
    cout<<endl;
    sort(recdata,recdata+localn);
    //iniciamos odd-even sort
    int oddrank,evenrank;
    if(rank%2==0){
    	oddrank=rank-1; 
    	evenrank = rank+1;
	}
 	else {
 		oddrank = rank+1;
 		evenrank = rank-1;
	}
	/* establecimos los rangos de los procesos */
	if (oddrank == -1 || oddrank == nump)
		oddrank = MPI_PROC_NULL;
	if (evenrank == -1 || evenrank == nump)
		evenrank = MPI_PROC_NULL;
	int p;
	for (p=0; p<nump-1; p++){//procesadores
		if (p%2 == 1) // Odd 
			MPI_Sendrecv(recdata, localn, MPI_INT, oddrank, 1, recdata2,localn, MPI_INT, oddrank, 1, MPI_COMM_WORLD, &status);
		else // Even 
			MPI_Sendrecv(recdata, localn, MPI_INT, evenrank, 1, recdata2,localn, MPI_INT, evenrank, 1, MPI_COMM_WORLD, &status);

		temp=new int[localn];
		for(i=0;i<localn;i++){
			temp[i]=recdata[i];
		}
		if(status.MPI_SOURCE==MPI_PROC_NULL)continue;
		else if(rank<status.MPI_SOURCE){
			int i,j,k;
			for(i=j=k=0;k<localn;k++){
				if(j==localn||(i<localn && temp[i]<recdata2[j]))
					recdata[k]=temp[i++];
				else
					recdata[k]=recdata2[j++];
			}
		}
		else{
			int i,j,k;
			for(i=j=k=localn-1;k>=0;k--){
				if(j==-1||(i>=0 && temp[i]>=recdata2[j]))
					recdata[k]=temp[i--];
				else
					recdata[k]=recdata2[j--];
			}
		}
	}
	timer = MPI_Wtime()-timer;
	timer= timer*1000;
	
	MPI_Gather(recdata,localn,MPI_INT,data,localn,MPI_INT,0,MPI_COMM_WORLD);
	
	/*if(rank==0){
	cout<<"array ordenado: ";
			for(i=0;i<n;i++){
				cout<<data[i]<<" ";
			}
		cout<<endl;
	}
	*/
	MPI_Finalize();
	cout<<"timer: "<<setprecision(5)<<timer<<endl;
}