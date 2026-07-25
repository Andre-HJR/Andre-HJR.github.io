==============
控制结构
==============

Fortran 的控制结构包括条件分支和循环。

条件分支
=========

if-else if-else
---------------

.. code-block:: fortran

   integer :: score = 85

   if (score >= 90) then
       write(*, *) "优秀"
   else if (score >= 80) then
       write(*, *) "良好"
   else if (score >= 60) then
       write(*, *) "及格"
   else
       write(*, *) "不及格"
   end if

单行 if
-------

.. code-block:: fortran

   ! 单行 if（无需 then/end if）
   if (x < 0) x = 0

   ! 逻辑 if（单条语句）
   if (flag) call do_something()

if 块嵌套
----------

.. code-block:: fortran

   if (condition1) then
       if (condition2) then
           ! 嵌套 if
           ...
       end if
   end if

.. note::

   每个 ``if`` 必须对应一个 ``end if``。
   ``else if`` 之间没有空格——连写为 ``elseif`` 也可。

select case 语句
=================

.. code-block:: fortran

   integer :: op = 2

   select case (op)
       case (1)
           write(*, *) "新增"
       case (2)
           write(*, *) "修改"
       case (3)
           write(*, *) "删除"
       case default
           write(*, *) "未知操作"
   end select

select case 的灵活用法
-----------------------

.. code-block:: fortran

   ! 范围匹配
   select case (score)
       case (:59)
           grade = "不及格"
       case (60:79)
           grade = "合格"
       case (80:89)
           grade = "良好"
       case (90:100)
           grade = "优秀"
       case default
           grade = "无效分数"
   end select

   ! 字符匹配
   character(1) :: cmd = "Q"
   select case (cmd)
       case ("N", "n")
           call new_file()
       case ("O", "o")
           call open_file()
       case ("Q", "q")
           call quit()
   end select

循环结构
=========

do 循环
--------

.. code-block:: fortran

   ! 基本 do 循环
   integer :: i
   do i = 1, 10
       write(*, *) i
   end do
   ! 输出: 1 2 3 ... 10

   ! 指定步长
   do i = 1, 10, 2
       write(*, *) i
   end do
   ! 输出: 1 3 5 7 9

   ! 递减
   do i = 10, 1, -1
       write(*, *) i
   end do
   ! 输出: 10 9 ... 1

   ! 不含循环变量的 do
   integer :: n = 0
   do
       n = n + 1
       if (n > 10) exit
   end do

do while 循环
--------------

.. code-block:: fortran

   integer :: i = 1
   do while (i <= 5)
       write(*, *) i
       i = i + 1
   end do

循环控制
=========

.. code-block:: fortran

   ! exit: 退出循环（相当于 C 的 break）
   do i = 1, 10
       if (i == 5) exit
       write(*, *) i
   end do
   ! 输出: 1 2 3 4

   ! cycle: 跳过当前迭代（相当于 C 的 continue）
   do i = 1, 10
       if (mod(i, 2) == 0) cycle
       write(*, *) i
   end do
   ! 输出: 1 3 5 7 9

.. caution::

   ``exit`` 和 ``cycle`` 只影响**当前最内层**循环。
   要跳出多层循环，可以使用命名循环（Fortran 95+）：

循环命名
---------

.. code-block:: fortran

   outer: do i = 1, 5
       inner: do j = 1, 5
           if (i * j > 10) exit outer
           write(*, *) i, j, i * j
       end do inner
   end do outer

数组循环（隐式 do）
====================

.. code-block:: fortran

   ! 隐式 do 用于 I/O 和数组构造
   integer :: arr(5)

   ! 读取数组
   read(*, *) (arr(i), i = 1, 5)

   ! 输出数组
   write(*, '(5I4)') (arr(i), i = 1, 5)

   ! 数组构造
   arr = (/ (i * 2, i = 1, 5) /)      ! arr = [2, 4, 6, 8, 10]

where 结构（数组条件操作）
============================

.. code-block:: fortran

   real :: a(10), b(10)

   ! 初始化
   a = (/ (i * 1.0, i = 1, 10) /)

   ! where: 对满足条件的数组元素执行操作
   where (a > 5.0)
       b = a * 2.0
   elsewhere
       b = a
   end where

forall 结构（Fortran 95+）
============================

.. code-block:: fortran

   integer :: mat(10, 10)

   ! forall: 并行赋值
   forall (i = 1:10, j = 1:10)
       mat(i, j) = i + j
   end forall

   ! 带条件的 forall
   forall (i = 1:10, j = 1:10, i /= j)
       mat(i, j) = 0
   end forall

.. note::

   ``forall`` 在 Fortran 2008 中已被标记为"过时"（deprecated），
   建议使用 ``do concurrent`` 替代：

   .. code-block:: fortran

      integer :: i, j
      do concurrent (i = 1:10, j = 1:10, i /= j)
          mat(i, j) = 0
      end do

综合示例：数值积分
=====================

.. code-block:: fortran
   :caption: 梯形法求积分

   program integrate
       implicit none
       integer, parameter :: n = 1000
       real(8) :: a, b, h, x, sum
       integer :: i

       a = 0.0_dp     ! 积分下限
       b = 1.0_dp     ! 积分上限
       h = (b - a) / n

       sum = 0.5_dp * (f(a) + f(b))
       do i = 1, n - 1
           x = a + i * h
           sum = sum + f(x)
       end do
       sum = sum * h

       write(*, '(A, F12.6)') "积分结果: ", sum

   contains
       real(8) function f(x)
           real(8), intent(in) :: x
           f = x**2            ! 求 x^2 在 [0,1] 上的积分
       end function f
   end program integrate
