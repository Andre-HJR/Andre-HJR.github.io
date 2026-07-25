===========================
IVF 并行计算与 MKL
===========================

IVF 2011 提供了多种并行计算方案，从自动并行到显式 OpenMP 和 Intel MKL。

Intel MKL 数学库
==================

Intel Math Kernel Library (MKL) 提供高度优化的数学函数。

配置 MKL
----------

.. code-block:: text

   项目属性 → Intel Performance Libraries → Use MKL → Parallel

   或链接器设置：
   附加依赖项: mkl_intel_lp64.lib mkl_core.lib mkl_intel_thread.lib
              libiomp5md.lib

BLAS 矩阵运算
--------------

.. code-block:: fortran

   program mkl_demo
       use blas95              ! Fortran 95 BLAS 接口
       implicit none
       integer, parameter :: n = 3
       real(8) :: a(n, n), b(n, n), c(n, n)

       ! 初始化矩阵
       a = reshape([1.0_dp, 2.0_dp, 3.0_dp, &
                    4.0_dp, 5.0_dp, 6.0_dp, &
                    7.0_dp, 8.0_dp, 9.0_dp], [n, n])
       b = reshape([9.0_dp, 8.0_dp, 7.0_dp, &
                    6.0_dp, 5.0_dp, 4.0_dp, &
                    3.0_dp, 2.0_dp, 1.0_dp], [n, n])

       ! 矩阵乘积 C = A * B
       call gemm(a, b, c)
       write(*, *) "矩阵乘法结果:"
       write(*, '(3F8.2)') c

       ! 线性方程组 Ax = b
       real(8) :: b_vec(n), x(n)
       b_vec = [1.0_dp, 2.0_dp, 3.0_dp]
       call gesv(a, b_vec, x)          ! 求解 Ax = b
       write(*, *) "解:", x
   end program

LAPACK 线性代数
----------------

.. code-block:: fortran

   program lapack_demo
       implicit none
       integer, parameter :: n = 3
       real(8) :: a(n, n), w(n)         ! w = 特征值
       integer :: info

       a = reshape([2.0_dp, -1.0_dp, 0.0_dp, &
                   -1.0_dp, 2.0_dp, -1.0_dp, &
                    0.0_dp, -1.0_dp, 2.0_dp], [n, n])

       ! 计算特征值 (dsyev = double symmetric eigenvalue)
       call dsyev("N", "U", n, a, n, w, work, lwork, info)

       if (info == 0) then
           write(*, *) "特征值:", w
       end if
   end program

.. tip::

   MKL 的向量数学库（VML）可以直接应用于数组：

   .. code-block:: fortran

      include 'mkl_vml.f90'
      real(8) :: x(1000), y(1000)
      x = [ (i * 0.01_dp, i = 1, 1000) ]
      call vdsin(1000, x, y)  ! 批量 sin 计算

OpenMP 并行
=============

IVF 2011 完全支持 OpenMP 3.0。

启用 OpenMP
------------

.. code-block:: text

   项目属性 → C/C++ → 语言 → OpenMP 支持 → 是 (/Qopenmp)
   或命令行: ifort /Qopenmp program.f90

并行 do 循环
-------------

.. code-block:: fortran

   program omp_demo
       use omp_lib            ! OpenMP 库模块
       implicit none
       integer, parameter :: n = 10000000
       real(8), allocatable :: a(:), b(:), c(:)
       real(8) :: start, end_time
       integer :: i

       allocate(a(n), b(n), c(n))
       a = 1.0_dp; b = 2.0_dp

       ! 计时
       start = omp_get_wtime()

       ! 并行 for 循环
       !$omp parallel do
       do i = 1, n
           c(i) = a(i) * b(i) + sin(a(i)) * cos(b(i))
       end do
       !$omp end parallel do

       end_time = omp_get_wtime()
       write(*, '(A, F8.4, A)') "并行耗时: ", end_time - start, " 秒"

   end program

.. important::

   OpenMP 指令格式：
   - ``!$omp``：通常指令
   - ``!$omp parallel do``：并行 do 循环
   - ``!$omp end parallel do``：结束（可选，由缩进推断）

规约操作
---------

.. code-block:: fortran

   real(8) :: sum, a(n)
   a = [ (i * 1.0_dp, i = 1, n) ]

   sum = 0.0_dp
   !$omp parallel do reduction(+:sum)
   do i = 1, n
       sum = sum + a(i)
   end do
   !$omp end parallel do
   write(*, *) "总和:", sum

并行区域
---------

.. code-block:: fortran

   integer :: thread_id, num_threads

   !$omp parallel private(thread_id) shared(num_threads)
       thread_id = omp_get_thread_num()
       !$omp master
           num_threads = omp_get_num_threads()
           write(*, *) "线程数:", num_threads
       !$omp end master
       write(*, '(A, I3, A, I3)') "线程 ", thread_id, " / ", num_threads - 1
   !$omp end parallel

do concurrent（Fortran 2008+）
===============================

.. code-block:: fortran

   ! 编译器自动并行化提示
   integer :: i, j
   real :: mat(1000, 1000)

   do concurrent (i = 1:1000)
       do concurrent (j = 1:1000)
           mat(i, j) = sin(i * 0.01_dp) * cos(j * 0.01_dp)
       end do
   end do

   ! do concurrent 要求：
   ! 1. 循环间无依赖
   ! 2. 不能有 I/O 语句
   ! 3. 不能有 return/exit
   ! 4. 不能修改循环外变量（除赋值外）

.. note::

   ``do concurrent`` 是**语义提示**，编译器可能自动并行，
   但不保证一定并行。与 OpenMP 结合使用效果更佳。

自动并行化（IVF 编译器）
===========================

IVF 可以自动分析循环并生成并行代码：

.. code-block:: text

   ifort /Qparallel program.f90       ! 自动并行
   ifort /Qpar-report:2 program.f90   ! 显示并行报告

.. caution::

   自动并行依赖编译器的分析结果，复杂循环可能无法自动并行。
   对于关键循环，建议使用显式的 OpenMP 指令。

并行性能建议
==============

.. list-table::
   :header-rows: 1

   * - 建议
     - 说明
   * - 大粒度并行
     - 每个线程分配足够大的工作量，减少调度开销
   * - 避免假共享
     - 不同线程的变量不要在同一缓存行上
   * - 减少同步
     - 用 ``reduction`` 代替显式临界区
   * - 数据本地化
     - 使用 ``private`` 声明临时变量，避免竞争
   * - 绑定线程
     - 设置 KMP_AFFINITY 环境变量绑定物理核心
   * - 均衡负载
     - 用 ``schedule(dynamic)`` 处理不均匀循环
   * - 嵌套并行谨慎
     - 外层已并行时，内层不再开新线程
   * - 预热
     - 在大循环前执行小计算让线程池激活

环境变量
=========

.. code-block:: text

   set OMP_NUM_THREADS=4        ! 设置线程数
   set KMP_AFFINITY=granularity=fine,compact,1,0  ! 线程亲核性
   set MKL_NUM_THREADS=4        ! MKL 线程数
   set KMP_BLOCKTIME=200        ! 线程等待时间（毫秒）
