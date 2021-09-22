#include <assert.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <cstdlib> 
#include <stdio.h>
#include <mpi.h> 
# define N_DIM 400  
/*compiler: mpicxx -o matrix matrix.cpp
  execution:   mpirun -np 4 ./matrix
*/
using namespace std;
void read_file(string s, int n_row, int n_col, double  *matrix){
	std::ifstream fin(s);
	std::string line;
	double data;
	if(!fin){
		cout << "Unable to open " << s << " for reading.\n";
		exit (0);
	}//leer mis datos de la matrix que la tengo en un txt
	for (int i = 0; i < n_row; i++){//filas
		//getline(fin, line);
	    //std::stringstream stream(line);
		for (int j = 0; j < n_col; j++){//columnas
			//stream >> data;      
			//*(matrix+(i*n_col)+j) = data;
            *(matrix+(i*n_col)+j)=rand()%20;
		}  
	}  
}  
void Multiply(int dim, double *matrix_data, double *vector_data,double *result){
    int rank,size;
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size); 
    double* ans = new double[dim / size]{};
    double matrix [dim][dim];   //matrix
    double timer=MPI_Wtime();//calcular el tiempo
    MPI_Barrier(MPI_COMM_WORLD);
    //scatter envia la matrix dividida, en el segundo argumento indicó cuánta cantidad de datos voy a enviar al proceso
    MPI_Scatter(matrix_data, (dim*dim)/size, MPI_DOUBLE, matrix, (dim*dim)/size, MPI_DOUBLE, 0, MPI_COMM_WORLD); 
    MPI_Bcast(vector_data, dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    //calcular resultados
    for (int i = 0;i<(dim/size);i++)
        for (int j = 0;j<dim;j++)
            ans[i]+=vector_data[j]*matrix[i][j];
    // allgather para que cada procesador tenga la respuesta 
    MPI_Allgather(ans, (dim)/size, MPI_DOUBLE, result, (dim)/size, MPI_DOUBLE, MPI_COMM_WORLD); 
    timer = MPI_Wtime()-timer;
    cout << "Timer = "<<timer*1000<<endl;
}
int main( int argc, char *argv[]){
    int rank, size;                    //MPI RANK,SIZE
    
    MPI_Init (&argc, &argv);                //Initializations
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size); 
    if (N_DIM%size){   //el número de procesos debe ser par
        MPI_Finalize();
        return(0);
    }
    
    double matrix_data[N_DIM][N_DIM];  //matrix
    double vector_data[N_DIM];         //vector
    double result[N_DIM] = {0.0};   //para el resultado final
 
    
    if (rank==0){
        read_file("matrix.txt", N_DIM, N_DIM, (double *)matrix_data);   //recolectar mi matrix
        read_file("vector.txt", N_DIM, 1, vector_data);                 //recolectar mi Vector
    }
    Multiply(N_DIM, (double *)matrix_data, vector_data,result);
    /* Impresión de la matrix*/
    /*if (rank==0){
        cout<<"Matrix  :\n";;
        for (int i=0;i<N_DIM;i++){
            for (int j=0;j<N_DIM;j++)
                cout<<matrix_data[i][j]<<" ";
            cout<<endl;
        }
        cout<<"Vector :\n";
        for (int i=0;i<N_DIM;i++)
            cout<<vector_data[i]<<" ";
        cout<<endl;
        cout<<"Result :\n";
        for (int i=0;i<N_DIM;i++)
            cout<<result[i]<<" ";
        cout<<endl;
    }*/
    MPI_Finalize();
    return(0);
}

