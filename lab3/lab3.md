# <table><td bgcolor="#FF00FF"><center><font face="Consolas" size=7>lab3</font></center></td></table>

<p align="right"><font face="Consolas">韩佳乐 PB16051152</font></p>

***

## <font face="Consolas" size=5>实验题目</font>

<font face="Consolas">

1. 题目：
利用MPI解决N体问题
2. 实验内容：
N体问题是指找出已知初始位置、速度和质量的多个物体在经典力学情况下的后续运动。在本次实验中，你需要模拟N个物体在二维空间中的运动情况。通过计算每两个物体之间的相互作用力，可以确定下一个时间周期内的物体位置。
在本次实验中，初始情况下，N个小球等间隔分布在一个正方形的二维空间中，小球在运动时没有范围限制。每个小球间会且只会受到其他小球的引力作用。小球可以看成质点。小球移动不会受到其他小球的影响(即不会发生碰撞，挡住等情况)。你需要计算模拟一定时间后小球的分布情况，并通过MPI并行化计算过程。
3. 实验要求
   1. 有关参数要求如下：
      * 引力常数数值取6.67*10^11
      * 小球重量都为10000kg
      * 小球间的初始间隔为1cm，例：N=36时，初始的正方形区域为5cm*5cm
      * 小球初速为0
      * 其他未定义的参数均可自行拟定
   2. 你的程序中，应当实现下面三个函数：
      * compute force(): 计算每个小球受到的作用力
      * compute velocities(): 计算每个小球的速度
      * compute positions(): 计算每个小球的位置

      典型的程序中，这三个函数应该是依次调用的关系。如果你的方法中不实现这三个函数，应当在报告中明确说明，并解释你的方法为什么不需要上述函数的实现。
   3. 报告中需要有 N=64 和 N=256 的情况下通过调整并行度计算的 程序执行时间和加速比

</font>

## <font face="Consolas" size=5>实验环境</font>

<font face="Consolas">

<table style="word-break:break-all;">
    <tr>
        <th>操作系统</th>
        <th>编译器</th>
        <th>硬件配置</th>
    </tr>
    <tr>
        <td><center>Ubuntu 16.04</center></td>
        <td><center>mpicc</center></td>
        <td><center>双核 4G内存</center></td>
    </tr>
</table>

</font>

## <font face="Consolas" size=5>算法设计与分析</font>

### <font face="Consolas" size=4>初始化条件</font>

<font face="Consolas">

初始情况下，N个小球等间隔分布在一个正方形的二维空间中，小球在运动时没有范围限制。每个小球间会且只会受到其他小球的引力作用。小球可以看成质点。小球间的初始间隔为1cm

</font>

### <font face="Consolas" size=4>小球结构体</font>

<font face="Consolas">

1. px和py为位置
2. ax和ay为加速度的水平和竖直分量
3. vx和vy为速度的水平和竖直分量

</font>

```c
typedef struct object
{
    double px, py;
    double ax, ay;
    double vx, vy;
}object;
```

### <font face="Consolas" size=4>计算作用力(加速度)</font>

<font face="Consolas">

d为两球间距离。R为小球半径，d的最小值为2R。

</font>

```c
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
```

### <font face="Consolas" size=4>计算速度</font>

<font face="Consolas">

delta_t是时间间隔，设置为0.001。理论上，该值越小，计算越精确。

</font>

```c
void compute_velocity(object *list, int i)
{
    list[i].vx += list[i].ax * delta_t;
    list[i].vy += list[i].ay * delta_t;
}
```

### <font face="Consolas" size=4>计算位置</font>

<font face="Consolas">

delta_t是时间间隔，设置为0.001。理论上，该值越小，计算越精确。

</font>

```c
void compute_position(object *list, int i)
{
    list[i].px += list[i].vx * delta_t;
    list[i].py += list[i].vy * delta_t;
}
```

### <font face="Consolas" size=4>并行设计</font>

<font face="Consolas">

1. 初始化MPI环境，获取并行环境参数(总线程数、本地进程编号等)。

    ```c
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    ```

2. 从0号进程获取输入数量和周期，并将其广播出去。

    ```c
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
    MPI_Bcast(&l, 1, MPI_INT, 0, MPI_COMM_WORLD);   //将l值广播出去
    ```

3. 初始化列表。

    ```c
    object *list = (object*)malloc(n * sizeof(object));
    for(int i = 0; i < n; i++){
        list[i].vx = 0;
        list[i].vy = 0;
        list[i].px = i%l*0.01;
        list[i].py = i/l*0.01;
    }
    ```

</font>

### <font face="Consolas" size=4>核心循环</font>

<font face="Consolas">

1. 由于计算加速度时需要使用位置信息，所以更新加速度时不能同时更新位置信息，需要放在不同的循环中。
2. 每个进程需要进程间通信以获取计算所需的位置信息。

</font>

```c
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

}
```

## <font face="Consolas" size=5>结果统计</font>

<font face="Consolas">

<table align="center">
    <tr>
        <td><center><img src="./picture/64-初始.png" width="200">N=64 初始状态</center></td>
        <td><center><img src="./picture/64-1000.png" width="200">N=64 1000周期</center></td>
        <td><center><img src="./picture/64-2000.png" width="200">N=64 2000周期</center></td>
    </tr>
</table>

<table align="center">
    <tr>
        <td><center><img src="./picture/256-初始.png" width="200">N=256 初始状态</center></td>
        <td><center><img src="./picture/256-1000.png" width="200">N=256 1000周期</center></td>
        <td><center><img src="./picture/256-2000.png" width="200">N=256 2000周期</center></td>
    </tr>
</table>

<table style="word-break:break-all;">
    <tr>
        <th>规模/线程数/运行时间(s)</th>
        <th>1</th>
        <th>2</th>
    </tr>
    <tr>
        <td><center>N=64 1000周期</center></td>
        <td><center>0.807149</center></td>
        <td><center>0.492253</center></td>
    </tr>
    <tr>
        <td><center>N=64 2000周期</center></td>
        <td><center>1.612149</center></td>
        <td><center>0.964136</center></td>
    </tr>
    <tr>
        <td><center>N=256 1000周期</center></td>
        <td><center>12.473251</center></td>
        <td><center>7.423541</center></td>
    </tr>
    <tr>
        <td><center>N=256 2000周期</center></td>
        <td><center>24.889742</center></td>
        <td><center>14.690790</center></td>
    </tr>
</table>

<table style="word-break:break-all;">
    <tr>
        <th>规模/线程数/加速比</th>
        <th>1</th>
        <th>2</th>
    </tr>
    <tr>
        <td><center>N=64 1000周期</center></td>
        <td><center>1</center></td>
        <td><center>1.640</center></td>
    </tr>
    <tr>
        <td><center>N=64 2000周期</center></td>
        <td><center>1</center></td>
        <td><center>1.672</center></td>
    </tr>
    <tr>
        <td><center>N=256 1000周期</center></td>
        <td><center>1</center></td>
        <td><center>1.680</center></td>
    </tr>
    <tr>
        <td><center>N=256 2000周期</center></td>
        <td><center>1</center></td>
        <td><center>1.694</center></td>
    </tr>
</table>

</font>

## <font face="Consolas" size=5>分析与总结</font>

<font face="Consolas">

1. 实验中没有4线程和8线程的数据，主要是因为实验时发现这两种情况下运行时间非常久，严重不符合预期。至于原因，暂未找到。
2. 时间间隔delta_t表示每个周期的间隔，其值会影响小球分布形状。当delta_t=0.001时，小球分布图如上图所示，当delta_t=0.01时，小球分布图更趋于圆形。

</font>
