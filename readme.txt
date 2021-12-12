1 课程主页lec2_PP.ppt的P35-36两种重复计算方式的OpenMP和MPI实现。（任务数可约定为2的幂次方） 
2 前期练习作业题目中的相关程序实现（ex-21-1/2/3 中有要求向量化/并行化的程序实习）。 
3 新的广播MyBcastMPI实现。基本思路：（1）将MPI进程按所在节点划分子通讯域N；（2）可以将各子通讯域的首进程（编号为0）再组成一个子通讯域H；（3）由广播的root进程将消息发给原来最大通讯域中的0号进程h，再由h在H通讯域中广播（MPI_Bcast），各首进程然后在各自子通讯域N中再行广播（MPI_Bcast）。 
4 用MPI_Send和MPI_Recv来模拟实现诸如MPI_Alltoall, MPI_Allgather功能并与标准MPI实现做简要性能对比。


（a） 在教材中 18.5 节 LU 分解的并行 MPI 实现基础上，给出 MPI+OpenMP 混 合实现；
（b） 针对教材中 18.6 节 QR 分解，给出纯 OpenMP 实现；
（c） SUMMA 并行矩阵乘法的 MPI 实现。参看文件 summa_2010.pdf 和原始文 章 lawn96.pdf。 
（d） 结合自己研究方向中一个小问题的并行实现（MPI 或 OpenMP 或混合，三选一）