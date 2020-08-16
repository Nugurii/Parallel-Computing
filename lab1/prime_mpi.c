#include "mpi.h"
#include <stdio.h>
#include <math.h>
 
int isPrime(int n)   //判断是否为素数  
{
    int flag = 1;
    int m = sqrt((double)n);
    for(int j = 2; j <= m; j++){
        if(n % j == 0){  
            flag = 0;
            break;
        }
    }
    return flag;
}  
 
int main(int argc, char* argv[])
{
	int n = 0, myid, numprocs, pi, sum, mypi;
	double startwtime, endwtime;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	if(myid == 0){
		printf("输入n:");
		scanf("%d",&n);
		startwtime = MPI_Wtime();
	}
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);   //将n值广播出去
	sum = 0;
	for(int i = myid*2+1; i <= n; i += numprocs*2){
		sum += isPrime(i);
    }
	mypi = sum;
    // printf("%d, %d\n", myid, sum);
	MPI_Reduce(&mypi, &pi, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);  //规约
	if(myid == 0){
		printf("并行结果:%d\n", pi);
		endwtime = MPI_Wtime();
		printf("并行时间:%f\n", endwtime - startwtime);
	}
	//串行程序
	// sum = 0;
	// double startwtime2 = MPI_Wtime();
	// if(myid == 0){
	// 	for(int i = 1; i <= n; i += 2){
	// 		sum += isPrime(i);
    //     }
	// 	double endwtime2 = MPI_Wtime();
	// 	printf("串行结果:%d\n", sum);
	// 	printf("串行时间:%f\n", endwtime2 - startwtime2);
	// }
 
	MPI_Finalize();
}