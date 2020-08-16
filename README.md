# Ubuntu 16.04 安装 MPI(Message Passing Interface)

## 1. 下载源文件

- 镜像网站 [https://www.mpich.org/static/downloads/3.3.2/](https://www.mpich.org/static/downloads/3.3.2/)
- 官方网站 [http://www.mpich.org/downloads/](http://www.mpich.org/downloads/)

## 2. 解压与安装

1. 解压

    ```c
    sudo tar -zxvf mpich-3.3.2.tar.gz
    cd mpich-3.3.2
    ```

2. 安装路径

    ```c
    ./configure -prefix=/usr/local/mpich-3.3.2
    ```

3. 安装

    ```c
    sudo make
    sudo make install
    ```

4. 配置环境变量

    ```c
    sudo gedit ~/.profile
    export PATH=/usr/local/mpich-3.3.2/bin:$PATH
    source ~/.profile
    ```

5. 如果使用vscode，还需配置头文件mpi.h的路径。`ctrl+shift+p` 后搜索 `conf` ，回车即可打开配置。

    ```c
    "includePath": [
        "${workspaceFolder}/**",
        "/usr/local/mpich-3.3.2/include"
    ],
    ```

## 3. 运行

1. hello.c

    ```c
    #include "mpi.h"
    #include <stdio.h>

    int main (int argc, char **argv)
    {
        int myid, numprocs;
        MPI_Init (&argc, &argv);
        MPI_Comm_rank (MPI_COMM_WORLD, &myid);
        MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
        printf ("Hello World! Process %d of %d\n", myid, numprocs);
        MPI_Finalize ();
        return 0;
    }
    ```

2. 运行

    ```c
    mpicc hello.c -o hello
    mpirun -np 4 ./hello
    ```

3. 结果

    ```c
    Hello World! Process 2 of 4
    Hello World! Process 3 of 4
    Hello World! Process 0 of 4
    Hello World! Process 1 of 4
    ```

## 4. 记录时间

`MPI_Wtime()` ✔<br>
`clock()` ❌
