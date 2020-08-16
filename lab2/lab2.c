#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define V_MAX 10
#define P     5
#define D_MAX 0x7fffffff

typedef struct
{
    int v;
    int pos;
    int d;
}car;

int main(int argc, char* argv[])
{
    int n, t, myid, numprocs, sum[11], res[11];
    double start, end;
    car *list;

    MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    for(int i = 0; i < 11; i++) {
        sum[i] = 0;
        res[i] = 0;
    }

    if(myid == 0){
        printf("车辆数量:");
        scanf("%d", &n);
        printf("周期:");
        scanf("%d", &t);
        start = MPI_Wtime();
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);   //将n值广播出去
    MPI_Bcast(&t, 1, MPI_INT, 0, MPI_COMM_WORLD);   //将t值广播出去

    list = (car*)malloc(n * sizeof(car));
    for(int i = 0; i < n; i++){
        list[i].v = 0;
        list[i].d = 0;
        list[i].pos = n - 1 - i;
    }
    list[0].d = D_MAX;

    for(int j = 0; j < t; j++){
        int k = n / numprocs * (myid + 1) - 1;
        if(myid != numprocs - 1){
            MPI_Send(&list[k].pos, 1, MPI_INT, myid + 1, myid, MPI_COMM_WORLD);
        }
        for(; k > n / numprocs * myid; k--){
            if(list[k].d > list[k].v && list[k].v < V_MAX) list[k].v++;
            if(list[k].d <= list[k].v) list[k].v = list[k].d;
            srand(k * j + clock());
            if(list[k].v >= 1){
                if(rand() % 10 < P) list[k].v--;
            }
            list[k].pos += list[k].v;
            list[k].d = list[k - 1].pos - list[k].pos;
        }

        if(list[k].d > list[k].v && list[k].v < V_MAX) list[k].v++;
        if(list[k].d <= list[k].v) list[k].v = list[k].d;
        srand(k * j + clock());
        if(list[k].v >= 1){
            if(rand() % 10 < P) list[k].v--;
        }
        list[k].pos += list[k].v;

        if(myid != 0){
            int front_car_pos;
            MPI_Status stat;
            MPI_Recv(&front_car_pos, 1, MPI_INT, myid - 1, myid - 1, MPI_COMM_WORLD, &stat);
            list[k].d = front_car_pos - list[k].pos;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for(int k = n / numprocs * myid; k < n / numprocs * (myid + 1); k++){
        sum[list[k].v]++;
    }
    MPI_Reduce(&sum, &res, 11, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);  //规约
    
    if(myid != 0){
        MPI_Send(list + n / numprocs * myid, n / numprocs * sizeof(car), MPI_BYTE, 0, myid, MPI_COMM_WORLD);
    }
    else{
        for(int i = 1; i < numprocs; i++){
            MPI_Status stat;
            MPI_Recv(list + n / numprocs * i, n / numprocs * sizeof(car), MPI_BYTE, i, i, MPI_COMM_WORLD, &stat);
        }
        end = MPI_Wtime();
        printf("并行时间:%f\n", end - start);
        FILE *fp = fopen("data.txt", "w");
        for(int i = 0; i < n; i++){
            fprintf(fp, "%20d %20d %20d\n", list[i].v, list[i].pos, list[i].d);
        }
        fclose(fp);
        for(int i = 0; i < 11; i++)
            printf("%d\n", res[i]);
    }

    free(list);
    MPI_Finalize();
}
