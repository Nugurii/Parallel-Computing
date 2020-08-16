# <center><font face="Consolas" size=7>lab1</font></center>

<div align="right"><font face="Consolas">韩佳乐 PB16051152</font></div>

***

## <font face="Consolas" size=5>实验题目</font>

<font face="Consolas">

1. 实验题目
利用 MPI,OpenMP 编写简单的程序,测试并行计算系统性能
2. 实验内容
两道题,每道题需要使用 MPI 和 OpenMP 分别实现:
    1. 求素数个数
        1. 实验描述:
        给定正整数 n,编写程序计算出所有小于等于 n 的素数的个数
        2. 实验要求:
        需要测定 n=1,000;10,000;100,000;500,000(逗号仅为清晰考虑)时程序运行的时间
    2. 求 Pi 值
        1. 实验描述:
        给定迭代次数 n,编写程序计算 Pi 的值
        2. 实验要求:
        算法必须使用近似公式求解。需要测定 n=1,000;10,000;50,000;100,000(逗号仅为清晰考虑)时程序运行的时间

</font>

## <font face="Consolas" size=5>实验环境</font>

<font face="Consolas">

<table width="100%">
    <tr>
        <th>操作系统</th>
        <th>编译器</th>
        <th>硬件配置</th>
    </tr>
    <tr>
        <td><center>Ubuntu 16.04</center></td>
        <td><center>gcc mpicc</center></td>
        <td><center>双核 4G内存</center></td>
    </tr>
</table>

</font>

## <font face="Consolas" size=5>算法设计与分析</font>

### <font face="Consolas" size=4>求素数个数</font>

<font face="Consolas">

检测一个整数a是否为素数：如果从2到$\sqrt{a}$均无法整除a，那么a即为素数。由于除2以外的所有偶数都不是素数，所以只需要检测1~n中所有的奇数(这里把1当作素数，把2当作非素数，虽然有违常理，但对求素数个数没有影响)。

</font>

### <font face="Consolas" size=4>求 Pi 值</font>

<img src="https://img-blog.csdnimg.cn/20200815230402166.png" width=70%>

## <font face="Consolas" size=5>核心代码</font>

### <font face="Consolas" size=4>求素数个数 MPI</font>

<font face="Consolas">

初始化MPI环境，获取并行环境参数(总线程数、本地进程编号等)。

```c
MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
MPI_Comm_rank(MPI_COMM_WORLD, &myid);
```

从0号进程获取输入n，并将其广播出去。

```c
if(myid == 0){
    printf("输入n:");
    scanf("%d",&n);
    startwtime = MPI_Wtime();
}
MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);   //将n值广播出去
```

并行计算，然后将结果进行规约。pi即为最终结果

```c
sum = 0;
for(int i = myid*2+1; i <= n; i += numprocs*2){
    sum += isPrime(i);
}
mypi = sum;
MPI_Reduce(&mypi, &pi, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);  //规约
```

</font>

### <font face="Consolas" size=4>求素数个数 OpenMP</font>

<font face="Consolas">

openMP相对比较简单，omp_set_num_threads()设置线程数，#pragma omp parallel for语句开启并行化，reduction(+:sum)表示各线程的运行结果sum最终要进行加法运算。

```c
omp_set_num_threads(2);
#pragma omp parallel for reduction(+:sum)
for(int i = 1; i <= n; i += 2){
    sum += isPrime(i);
}
```

</font>

### <font face="Consolas" size=4>求 Pi 值 MPI</font>

<font face="Consolas">

初始化MPI环境，获取并行环境参数(总线程数、本地进程编号等)。

```c
MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
MPI_Comm_rank(MPI_COMM_WORLD, &myid);
```

从0号进程获取输入n，并将其广播出去。

```c
if(myid == 0){
    printf("输入n:");
    scanf("%d",&n);
    startwtime = MPI_Wtime();
}
MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);   //将n值广播出去
```

并行计算，然后将结果进行规约。pi即为最终结果

```c
sum = 0;
for(int i = myid; i <= n; i += numprocs){
    sum += (i % 2 == 0 ? 4.0 : (-4.0)) / (2 * i + 1);
}
mypi = sum;
MPI_Reduce(&mypi, &pi, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);  //规约
```

</font>

### <font face="Consolas" size=4>求 Pi 值 OpenMP</font>

<font face="Consolas">

openMP相对比较简单，omp_set_num_threads()设置线程数，#pragma omp parallel for语句开启并行化，reduction(+:sum)表示各线程的运行结果sum最终要进行加法运算。

```c
omp_set_num_threads(2);
#pragma omp parallel for reduction(+:sum)
for(int i = 0; i <= n; i++){
    sum += (i % 2 == 0 ? 4.0 : (-4.0)) / (2 * i + 1);
}
```

## <font face="Consolas" size=5>实验结果</font>

<font face="Consolas">

<div align="center"><font size=2>求素数个数 MPI 运行时间(s)</font></div>

<table width="100%" align="center">
    <tr>
        <th width="20%">规模/线程数</th>
        <th width="20%">1</th>
        <th width="20%">2</th>
        <th width="20%">4</th>
        <th width="20%">8</th>
    </tr>
    <tr>
        <td><center>1000</center></td>
        <td><center>0.000075</center></td>
        <td><center>0.020078</center></td>
        <td><center>0.035705</center></td>
        <td><center>0.063428</center></td>
    </tr>
    <tr>
        <td><center>10000</center></td>
        <td><center>0.000652</center></td>
        <td><center>0.017054</center></td>
        <td><center>0.038764</center></td>
        <td><center>0.070721</center></td>
    </tr>
    <tr>
        <td><center>100000</center></td>
        <td><center>0.029157</center></td>
        <td><center>0.039937</center></td>
        <td><center>0.057087</center></td>
        <td><center>0.071655</center></td>
    </tr>
    <tr>
        <td><center>1000000</center></td>
        <td><center>0.404605</center></td>
        <td><center>0.334772</center></td>
        <td><center>0.280256</center></td>
        <td><center>0.238929</center></td>
    </tr>
    <tr>
        <td><center>5000000</center></td>
        <td><center>3.335149</center></td>
        <td><center>2.538442</center></td>
        <td><center>2.142986</center></td>
        <td><center>2.016233</center></td>
    </tr>
</table>

<div align="center"><font size=2>求素数个数 MPI 加速比</font></div>

<table width="100%" align="center">
    <tr>
        <th width="20%">规模/线程数</th>
        <th width="20%">1</th>
        <th width="20%">2</th>
        <th width="20%">4</th>
        <th width="20%">8</th>
    </tr>
    <tr>
        <td><center>1000</center></td>
        <td><center>1</center></td>
        <td><center>0.004</center></td>
        <td><center>0.002</center></td>
        <td><center>0.001</center></td>
    </tr>
    <tr>
        <td><center>10000</center></td>
        <td><center>1</center></td>
        <td><center>0.038</center></td>
        <td><center>0.017</center></td>
        <td><center>0.009</center></td>
    </tr>
    <tr>
        <td><center>100000</center></td>
        <td><center>1</center></td>
        <td><center>0.730</center></td>
        <td><center>0.511</center></td>
        <td><center>0.407</center></td>
    </tr>
    <tr>
        <td><center>1000000</center></td>
        <td><center>1</center></td>
        <td><center>1.209</center></td>
        <td><center>1.444</center></td>
        <td><center>1.693</center></td>
    </tr>
    <tr>
        <td><center>5000000</center></td>
        <td><center>1</center></td>
        <td><center>1.314</center></td>
        <td><center>1.556</center></td>
        <td><center>1.654</center></td>
    </tr>
</table>

<div align="center"><font size=2>求PI MPI 运行时间(s)</font></div>
<table width="100%" align="center">
    <tr>
        <th width="20%">规模/线程数</th>
        <th width="20%">1</th>
        <th width="20%">2</th>
        <th width="20%">4</th>
        <th width="20%">8</th>
    </tr>
    <tr>
        <td><center>10000</center></td>
        <td><center>0.000087</center></td>
        <td><center>0.004757</center></td>
        <td><center>0.025594</center></td>
        <td><center>0.040260</center></td>
    </tr>
    <tr>
        <td><center>100000</center></td>
        <td><center>0.000416</center></td>
        <td><center>0.015015</center></td>
        <td><center>0.036435</center></td>
        <td><center>0.054088</center></td>
    </tr>
    <tr>
        <td><center>1000000</center></td>
        <td><center>0.003892</center></td>
        <td><center>0.029813</center></td>
        <td><center>0.045457</center></td>
        <td><center>0.060422</center></td>
    </tr>
    <tr>
        <td><center>10000000</center></td>
        <td><center>0.047871</center></td>
        <td><center>0.037937</center></td>
        <td><center>0.055264</center></td>
        <td><center>0.070782</center></td>
    </tr>
    <tr>
        <td><center>100000000</center></td>
        <td><center>0.410810</center></td>
        <td><center>0.330348</center></td>
        <td><center>0.282296</center></td>
        <td><center>0.307018</center></td>
    </tr>
    <tr>
        <td><center>1000000000</center></td>
        <td><center>8.801296</center></td>
        <td><center>5.527966</center></td>
        <td><center>3.751501</center></td>
        <td><center>2.940126</center></td>
    </tr>
</table>

<div align="center"><font size=2>求PI MPI 加速比</font></div>

<table width="100%" align="center">
    <tr>
        <th width="20%">规模/线程数</th>
        <th width="20%">1</th>
        <th width="20%">2</th>
        <th width="20%">4</th>
        <th width="20%">8</th>
    </tr>
    <tr>
        <td><center>10000</center></td>
        <td><center>1</center></td>
        <td><center>0.018</center></td>
        <td><center>0.003</center></td>
        <td><center>0.002</center></td>
    </tr>
    <tr>
        <td><center>100000</center></td>
        <td><center>1</center></td>
        <td><center>0.028</center></td>
        <td><center>0.011</center></td>
        <td><center>0.008</center></td>
    </tr>
    <tr>
        <td><center>1000000</center></td>
        <td><center>1</center></td>
        <td><center>0.131</center></td>
        <td><center>0.086</center></td>
        <td><center>0.064</center></td>
    </tr>
    <tr>
        <td><center>10000000</center></td>
        <td><center>1</center></td>
        <td><center>1.262</center></td>
        <td><center>0.866</center></td>
        <td><center>0.676</center></td>
    </tr>
    <tr>
        <td><center>100000000</center></td>
        <td><center>1</center></td>
        <td><center>1.244</center></td>
        <td><center>1.455</center></td>
        <td><center>1.338</center></td>
    </tr>
    <tr>
        <td><center>1000000000</center></td>
        <td><center>1</center></td>
        <td><center>1.592</center></td>
        <td><center>2.346</center></td>
        <td><center>2.994</center></td>
    </tr>
</table>

</font>

## <font face="Consolas" size=5>分析与总结</font>

1. 当数据比较小时，进程的创建和同步等因多进程而导致的开销是加速比小于1的主要原因。随着数据的增大，计算的开销越来越大，这种额外开销占比越来越小，多线程并行计算的优点逐渐体现出来，因此加速比也随之增加并且超过1。

2. 当数据较大时，随着线程数的增加，加速比虽然也在增加，但是增加速率很小，部分原因是多线程导致的额外开销随着线程数增加而增加，更重要的原因在于当线程数超过CPU核数时(本实验为双核)CPU占有率会达到极限，很难再有提升。
