#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

int cmp ( const void *a , const void *b )
{
    return *(int *)a - *(int *)b;
}

void merge(int a[], int p, int q, int r){
    int n1 = q - p + 1;
    int n2 = r - q;
    int *b = (int*)malloc((n1+1)*sizeof(int));
    int *c = (int*)malloc((n2+1)*sizeof(int));
    memcpy(b, a + p, n1*sizeof(int));
    memcpy(c, a + q + 1, n2*sizeof(int));
    b[n1] = INT32_MAX;
    c[n2] = INT32_MAX;
    int i = 0, j = 0;
    for(int k = p; k <= r; k++){
        if(b[i] <= c[j]){
            a[k] = b[i];
            i++;
        }
        else{
            a[k] = c[j];
            j++;
        }
    }
    free(b);
    free(c);
}

void merge_sort(int a[], int p, int r, int pos[], int size[]){
    if(p < r){
        int q = (p + r) / 2;
        merge_sort(a, p, q, pos, size);
        merge_sort(a, q+1, r, pos, size);
        merge(a, pos[p], pos[q] + size[q] - 1, pos[r] + size[r] - 1);
    }
}

int main(int argc, char* argv[])
{
    int n, myid, numprocs;
    double start, end;

    MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if(myid == 0){
        printf("数量:");
        scanf("%d", &n);
        start = MPI_Wtime();
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);   // 将n值广播出去

    // 产生随机数组
    int *a = (int*)malloc(n*sizeof(int));
    if(myid == 0){
        for(int i = 0; i < n; i++){
            srand(i + clock());
            a[i] = rand() % 100;
        }
    }
    MPI_Bcast(a, n, MPI_INT, 0, MPI_COMM_WORLD);   // 将随机数组广播出去

    // 打印初始数组
    if(myid == 0){
        // printf("初始数组:\n");
        // for(int i = 0; i < n; i++){
        //     printf("%d ", a[i]);
        // }
        // printf("\n");
        start = MPI_Wtime();
    }

    // 各线程分别进行局部排序
    qsort(a+n/numprocs*myid, n/numprocs, sizeof(int), cmp);

    MPI_Barrier(MPI_COMM_WORLD);

    // 选取样本
    int *a1 = (int*)malloc(numprocs*numprocs*sizeof(int));
    for(int i = 0; i < numprocs; i++){
        a1[numprocs*myid + i] = a[n/numprocs*myid+n/(numprocs*numprocs)*i];
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for(int i = 0; i < numprocs; i++){
        MPI_Bcast(a1+numprocs*i, numprocs*sizeof(int), MPI_BYTE, i, MPI_COMM_WORLD);
    }

    // 选取主元
    int *a2 = (int*)malloc((numprocs - 1)*sizeof(int));

    if(myid == 0){
        qsort(a1, numprocs*numprocs, sizeof(int), cmp);
        for(int i = 0; i < numprocs - 1; i++){
            a2[i] = a1[numprocs*(i + 1)];
        }
    }

    // 把主元广播到所有进程
    MPI_Bcast(a2, (numprocs-1)*sizeof(int), MPI_BYTE, 0, MPI_COMM_WORLD);

    // 各个进程统计被主元切分的块大小
    int *psize = (int*)malloc(numprocs*sizeof(int));
    memset(psize, 0, numprocs*sizeof(int));
    int index = 0;
    for(int i = 0; i < n/numprocs; i++){
        if(a[n/numprocs*myid+i] > a2[index]){
            index += 1;
        }
        if(index == numprocs - 1){
            psize[numprocs - 1] = n/numprocs - i;
            break;
        }
        psize[index]++;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // 各进程统计要接收的块的大小
    int* psize1 = (int*)malloc(numprocs*sizeof(int));

    MPI_Alltoall(psize, 1, MPI_INT, psize1, 1, MPI_INT, MPI_COMM_WORLD);

    int totalSize = 0;
    for(int i = 0; i < numprocs; i++){
        totalSize += psize1[i];
    }

    // 全局交换
    int *a3 = (int*)malloc(totalSize*sizeof(int));
    int *sendPos = (int*)malloc(numprocs*sizeof(int));
    int *recvPos = (int*)malloc(numprocs*sizeof(int));
    sendPos[0] = 0;
    recvPos[0] = 0;
    for(int i = 1; i < numprocs; i++){
        sendPos[i] = sendPos[i - 1] + psize[i - 1];
        recvPos[i] = recvPos[i - 1] + psize1[i - 1];
    }
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Alltoallv(a+n/numprocs*myid, psize, sendPos, MPI_INT, a3, psize1, recvPos, MPI_INT, MPI_COMM_WORLD);

    // 归并排序
    merge_sort(a3, 0, numprocs - 1, recvPos, psize1);

    MPI_Barrier(MPI_COMM_WORLD);

    // 各进程把排序后的数组发送给根进程
    int *listSize = (int*)malloc(numprocs*sizeof(int));

    MPI_Gather(&totalSize, 1, MPI_INT, listSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(myid == 0){
        recvPos[0] = 0;
        for(int i = 1; i < numprocs; i++){
            recvPos[i] = recvPos[i - 1] + listSize[i - 1];
        }
    }

    MPI_Gatherv(a3, totalSize, MPI_INT, a, listSize, recvPos, MPI_INT, 0, MPI_COMM_WORLD);

    // 打印结果
    if(myid ==0 ){
        end = MPI_Wtime();
        // printf("排序后:\n");
        // for(int i = 0; i < n; i++){
        //     printf("%d ", a[i]);
        // }
        // printf("\n");
        int isRight = 1;
        for(int i = 0; i < n - 1; i++){
            if(a[i] > a[i + 1]) {
                isRight = 0;
                break;
            }
        }
        if(isRight)
            printf("耗时:%f\n", end - start);
        else
            printf("error\n");
    }

    free(a);
    free(a1);
    free(a2);
    free(a3);
    free(psize);
    free(psize1);
    free(listSize);
    free(sendPos);
    free(recvPos);

    MPI_Finalize();
}