#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

int main(int argc, char* argv[])
{
    int n, numprocs, myid;
    double sum, mypi, pi;
    double start, end;
	// clock_t start, end;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	if(myid == 0){
		printf("输入n:");
		scanf("%d",&n);
		start = MPI_Wtime();
		// start = clock();
	}
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);   //将n值广播出去

    sum = 0;
    for(int i = myid; i <= n; i += numprocs){
        sum += (i % 2 == 0 ? 4.0 : (-4.0)) / (2 * i + 1);
    }

    mypi = sum;
    MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);  //规约
	if(myid == 0){
		printf("并行结果:%f\n", pi);
		end = MPI_Wtime();
		// end = clock();
		printf("串行时间:%f\n", end - start);
		// printf("并行时间:%f\n", (double)(end - start)/CLOCKS_PER_SEC);
	}

	//串行程序
	// sum = 0;
	// double start2 = MPI_Wtime();
	// // clock_t start2 = clock();
	// if(myid == 0){
	// 	for(int i = 0; i <= n; i++){
	// 		sum += (i % 2 == 0 ? 4.0 : (-4.0)) / (2 * i + 1);
    //     }
	// 	double end2 = MPI_Wtime();
	// 	// double end2 = clock();
	// 	printf("串行结果:%f\n", sum);
	// 	printf("串行时间:%f\n", end2 - start2);
	// 	// printf("串行时间:%f\n", (double)(end2 - start2)/CLOCKS_PER_SEC);
	// }

    MPI_Finalize();
}