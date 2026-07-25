========
数组
========

数组是 Fortran 的**一等公民**，拥有极其强大的数组操作语法。

数组声明
=========

静态数组
---------

.. code-block:: fortran

   ! 一维数组
   integer  :: arr1(10)           ! 10 个整数, 索引 1..10
   real(8)  :: arr2(0:9)          ! 索引 0..9（显式指定下界）
   real     :: arr3(-5:5)         ! 索引 -5..5

   ! 二维数组
   integer  :: mat1(3, 4)         ! 3 行 4 列
   real(8)  :: mat2(0:2, 0:3)    ! 指定索引范围

   ! 三维数组
   real :: cube(3, 3, 3)

   ! 使用参数常量
   integer, parameter :: N = 100, M = 200
   real(8) :: data(N, M)

.. note::

   **Fortran 数组默认索引从 1 开始**，与 C 语言不同！
   使用 ``0:9`` 语法可改为 0 基索引。

可分配数组（动态数组）
-----------------------

.. code-block:: fortran

   ! 声明可分配数组
   integer, allocatable :: arr(:)
   real(8), allocatable :: mat(:, :)

   ! 分配空间
   allocate(arr(100))
   allocate(mat(50, 100))

   ! 使用后释放
   deallocate(arr)
   deallocate(mat)

   ! 检查分配状态
   if (allocated(arr)) then
       write(*, *) "数组已分配，大小:", size(arr)
   end if

.. tip::

   可分配数组会在过程结束时**自动释放**（Fortran 2003+），
   但显式调用 ``deallocate`` 仍是好习惯。

自动数组
---------

.. code-block:: fortran

   subroutine process(n)
       integer, intent(in) :: n
       ! 自动数组：在栈上分配，过程结束后自动释放
       real :: local_arr(n)
       ...
   end subroutine

数组初始化
===========

.. code-block:: fortran

   ! 直接赋值
   integer :: a(5) = [1, 2, 3, 4, 5]

   ! 数组构造器（Fortran 2003+）
   integer :: b(5) = (/ 1, 2, 3, 4, 5 /)   ! 旧式
   integer :: c(5) = [ 1, 2, 3, 4, 5 ]     ! 新式（推荐）

   ! 等间距数组
   real :: d(5) = [ (i * 2.0, i = 1, 5) ]

   ! 全部初始化为相同值
   real :: e(100) = 0.0

   ! 二维数组初始化
   integer :: f(2, 3) = reshape([1, 2, 3, 4, 5, 6], [2, 3])
   ! f(1,1)=1  f(1,2)=3  f(1,3)=5
   ! f(2,1)=2  f(2,2)=4  f(2,3)=6

.. important::

   Fortran 数组按**列优先**顺序存储：
   ``f(1,1)`` → ``f(2,1)`` → ``f(1,2)`` → ``f(2,2)`` → ...

   这恰好与 C 语言的行优先相反。在调用 BLAS/LAPACK 等库时需特别注意。

数组操作
=========

元素访问
---------

.. code-block:: fortran

   integer :: a(10), mat(3, 4)

   ! 单个元素
   a(1) = 10
   mat(2, 3) = 42

   ! 数组切片（section）
   a(1:5) = 1               ! a(1)..a(5) 全部设为 1
   a(2:8:2) = 0             ! a(2), a(4), a(6), a(8) 设为 0
   a(:) = 0                 ! 全部清零

   mat(1, :) = [1, 2, 3, 4]     ! 第一行
   mat(:, 2) = [5, 6, 7]        ! 第二列
   mat(1:2, 1:2) = 0             ! 子矩阵

整体运算
---------

.. code-block:: fortran

   real :: a(10), b(10), c(10)

   a = 1.0                ! 标量广播
   b = [ (i * 2.0, i = 1, 10) ]

   ! 逐元素运算
   c = a + b              ! 加法
   c = a - b              ! 减法
   c = a * b              ! 逐元素乘法（不是矩阵乘法！）
   c = a / b              ! 逐元素除法
   c = a ** 2             ! 逐元素幂
   c = sin(a)             ! 逐元素三角运算
   c = sqrt(abs(a))       ! 逐元素组合运算

   ! 条件赋值
   where (a > 0.5) b = a * 2.0

.. caution::

   ``a * b`` 是**逐元素乘法**（Hadamard 积），
   矩阵乘法需使用 ``matmul(a, b)`` 函数。

内置数组函数
=============

.. code-block:: fortran

   real :: a(100), mat(10, 10)

   ! 形状查询
   size(a)                  ! 100（元素总数）
   size(mat, dim=1)         ! 10（第一维大小）
   shape(mat)               ! [10, 10]（形状数组）
   rank(mat)                ! 2（维数）
   allocated(a)             ! .true./.false.

   ! 边界查询
   lbound(a)                ! 1（下界）
   ubound(a)                ! 100（上界）

   ! 归约
   sum(a)                   ! 求和
   product(a)               ! 求积
   maxval(a)                ! 最大值
   minval(a)                ! 最小值
   maxloc(a)                ! 最大值位置
   minloc(a)                ! 最小值位置

   ! 矩阵运算
   matmul(m1, m2)           ! 矩阵乘法
   transpose(mat)           ! 转置
   dot_product(v1, v2)      ! 向量点积

   ! 重排
   pack(arr, mask)          ! 按掩码选取
   unpack(...)              ! 反向操作
   reshape(arr, shape)      ! 改变形状
   cshift(arr, shift)       ! 循环位移
   eoshift(arr, shift)      ! 边界位移

数组作为参数
=============

.. code-block:: fortran

   ! 假定形状数组（常用推荐方式）
   subroutine process(arr)
       integer, intent(inout) :: arr(:)     ! 一维假定形状
       ! arr 的索引从 1 开始（除非调用时指定下界）
       write(*, *) "大小:", size(arr)
   end subroutine

   ! 显式形状数组
   subroutine fixed_arr(arr)
       integer, intent(in) :: arr(10, 20)   ! 固定大小
   end subroutine

   ! 假定大小数组（旧式，不推荐）
   subroutine old_style(arr)
       integer :: arr(*)                     ! 不安全
   end subroutine

.. tip::

   **假定形状数组** ``arr(:)`` 是最佳实践：
   - 自动传递数组形状信息
   - 利用 ``size``、``lbound`` 等查询
   - 不限制调用方的数组大小

可分配数组作为过程参数
------------------------

.. code-block:: fortran

   subroutine create_array(arr, n)
       integer, allocatable, intent(out) :: arr(:)
       integer, intent(in) :: n

       allocate(arr(n))
       arr = [ (i, i = 1, n) ]
   end subroutine

   program main
       implicit none
       integer, allocatable :: arr(:)

       call create_array(arr, 10)
       write(*, *) arr       ! [1, 2, 3, ..., 10]
       deallocate(arr)
   end program main
