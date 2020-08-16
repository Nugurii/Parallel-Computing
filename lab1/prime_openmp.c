#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>

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

int main() 
{
    int n, x, sum;
    // clock_t start, end;
    double start, end;
    printf("输入n:");
    scanf("%d",&n);

    printf("输入x:");
    scanf("%d",&x);

    // start = clock();
    start = omp_get_wtime();
    sum = 0;
    omp_set_num_threads(x);
    #pragma omp parallel for reduction(+:sum)
	for(int i = 1; i <= n; i += 2){
		sum += isPrime(i);
        // printf("%d, %d\n", i, omp_get_thread_num());
    }
    // end = clock();
    end = omp_get_wtime();
    printf("并行结果:%d\n", sum);
    // printf("并行时间:%f\n", (double)(end - start)/CLOCKS_PER_SEC);
    printf("并行时间:%f\n", end - start);

    // // start = clock();
    // start = omp_get_wtime();
    // sum = 0;
	// for(int i = 1; i <= n; i += 2){
	// 	sum += isPrime(i);
    // }
    // // end = clock();
    // end = omp_get_wtime();
    // printf("串行结果:%d\n", sum);
    // printf("串行时间:%f\n", end - start);
    // printf("串行时间:%f\n", (double)(end - start)/CLOCKS_PER_SEC);

    return 0;
}