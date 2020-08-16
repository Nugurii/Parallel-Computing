#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define G  6.67e-11
#define M  10000
#define R  0.0001
#define delta_t  0.001

typedef struct object
{
    double px, py;
    double ax, ay;
    double vx, vy;
}object;

void compute_force(object *list, int size, int i)
{
    list[i].ax = 0;
    list[i].ay = 0;
    for(int k = 0; k < size; k++){
        if(k == i) continue;
        double dx = list[k].px - list[i].px;
        double dy = list[k].py - list[i].py;
        double d = sqrt(dx*dx + dy*dy);
        if(d < 2*R) d = 2*R;
        list[i].ax += G*M*dx/pow(d, 3);
        list[i].ay += G*M*dy/pow(d, 3);
    }
}

void compute_velocity(object *list, int i)
{
    list[i].vx += list[i].ax * delta_t;
    list[i].vy += list[i].ay * delta_t;
}

void compute_position(object *list, int i)
{
    list[i].px += list[i].vx * delta_t;
    list[i].py += list[i].vy * delta_t;
}

int main(int argc, char* argv[])
{
    int n, l, t, myid, numprocs, sum[11], res[11];
    double start, end;

    MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if(myid == 0){
        printf("数量:");
        scanf("%d", &n);
        l = (int)sqrt((double)n);
        printf("周期:");
        scanf("%d", &t);
        start = MPI_Wtime();
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);   //将n值广播出去
    MPI_Bcast(&t, 1, MPI_INT, 0, MPI_COMM_WORLD);   //将t值广播出去
    MPI_Bcast(&l, 1, MPI_INT, 0, MPI_COMM_WORLD);   //将t值广播出去

    object *list = (object*)malloc(n * sizeof(object));
    for(int i = 0; i < n; i++){
        list[i].vx = 0;
        list[i].vy = 0;
        list[i].px = i%l*0.01;
        list[i].py = i/l*0.01;
    }

    for(int j = 0; j < t; j++){
        for(int i = n/numprocs*myid; i < n/numprocs*(myid+1); i++){
            compute_force(list, n, i);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        for(int i = n/numprocs*myid; i < n/numprocs*(myid+1); i++){
            compute_velocity(list, i);
            compute_position(list, i);
        }
        MPI_Barrier(MPI_COMM_WORLD);

        for(int i = 0; i < numprocs; i++){
            if(i != myid)
                MPI_Send(list+(n/numprocs)*myid, n/numprocs*sizeof(object), MPI_BYTE, i, myid, MPI_COMM_WORLD);
        }
        
        for(int i = 0; i < numprocs; i++){
            if(i != myid){
                MPI_Status stat;
                MPI_Recv(list+(n/numprocs)*myid, n/numprocs*sizeof(object), MPI_BYTE, i, i, MPI_COMM_WORLD, &stat);
            }
        }
        for(int i = 0; i < numprocs; i++)
            MPI_Bcast(list+(n/numprocs)*i, n/numprocs*sizeof(object), MPI_BYTE, i, MPI_COMM_WORLD);
        
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if(myid == 0){

        end = MPI_Wtime();
        printf("并行时间:%f\n", end - start);

        FILE *fp = fopen("px.txt", "w");
        for(int i = 0; i < n; i++){
            fprintf(fp, "%f\n", list[i].px);
        }
        fclose(fp);

        fp = fopen("py.txt", "w");
        for(int i = 0; i < n; i++){
            fprintf(fp, "%f\n", list[i].py);
        }
        fclose(fp);
    }

    free(list);
    MPI_Finalize();
}