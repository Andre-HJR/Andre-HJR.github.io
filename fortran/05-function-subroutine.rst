=========================
函数与子程序
=========================

Fortran 中有两种过程的组织形式：**函数** (function) 和 **子程序** (subroutine)。

函数（Function）
=================

基本语法
---------

.. code-block:: fortran

   ! 函数定义
   function add(a, b) result(sum)
       implicit none
       integer, intent(in) :: a, b
       integer :: sum           ! 返回值

       sum = a + b
   end function add

   ! 调用
   integer :: x = 3, y = 4, z
   z = add(x, y)                ! z = 7

简写形式
---------

.. code-block:: fortran

   ! 省略 result 子句（函数名作为返回值变量）
   integer function square(x)
       implicit none
       integer, intent(in) :: x
       square = x * x
   end function square

.. tip::

   推荐总是使用 ``result`` 子句，这样函数名和返回值变量名分离更清晰。

intent 属性
============

.. list-table::
   :header-rows: 1

   * - 属性
     - 含义
     - 效果
   * - ``intent(in)``
     - 输入参数
     - 过程内不可修改
   * - ``intent(out)``
     - 输出参数
     - 进入过程时值未定义
   * - ``intent(inout)``
     - 输入输出参数
     - 可读可修改

.. code-block:: fortran

   subroutine calc(a, b, c)
       implicit none
       real, intent(in)    :: a      ! 只读
       real, intent(out)   :: b      ! 只写
       real, intent(inout) :: c      ! 可读可写

       b = a * 2.0
       c = c + a
   end subroutine

.. important::

   在可能的情况下**始终使用 intent** (标注 ``intent`` 属性)：
   - 帮助编译器检查错误
   - 生成更优化的代码
   - 提高代码自文档性

子程序（Subroutine）
=====================

子程序不返回值（但可以通过参数返回）。

.. code-block:: fortran

   subroutine swap(a, b)
       implicit none
       integer, intent(inout) :: a, b
       integer :: temp

       temp = a
       a = b
       b = temp
   end subroutine swap

   ! 调用
   integer :: x = 3, y = 5
   call swap(x, y)             ! x=5, y=3

函数 vs 子程序
===============

.. list-table::
   :header-rows: 1

   * - 特性
     - 函数 (function)
     - 子程序 (subroutine)
   * - 返回值
     - 以一个返回值为主
     - 无返回值，通过参数输出
   * - 调用方式
     - ``result = func(args)``
     - ``call sub(args)``
   * - 用法
     - 表达式内使用
     - 作为独立语句
   * - 适合场景
     - 数学计算、判断
     - 修改参数、I/O 操作

递归过程
=========

.. code-block:: fortran

   ! 递归函数
   recursive function factorial(n) result(res)
       implicit none
       integer, intent(in) :: n
       integer :: res

       if (n <= 1) then
           res = 1
       else
           res = n * factorial(n - 1)
       end if
   end function factorial

   ! 递归子程序
   recursive subroutine hanoi(n, from, to, aux)
       implicit none
       integer, intent(in) :: n
       character(1), intent(in) :: from, to, aux

       if (n == 1) then
           write(*, '(A, A, A, A)') "移动盘 1 从 ", from, " 到 ", to
       else
           call hanoi(n - 1, from, aux, to)
           write(*, '(A, I0, A, A, A, A)') "移动盘 ", n, " 从 ", from, " 到 ", to
           call hanoi(n - 1, aux, to, from)
       end if
   end subroutine hanoi

.. caution::

   递归函数必须使用 ``recursive`` 关键字！
   Fortran 95 之前的版本不支持递归。

pure 与 elemental 过程
========================

pure 过程
----------

不产生任何副作用的函数，可安全并行执行：

.. code-block:: fortran

   pure function square(x) result(res)
       implicit none
       real, intent(in) :: x
       real :: res
       res = x * x
   end function

elemental 过程
---------------

对标量定义，但可自动逐元素作用于数组：

.. code-block:: fortran

   elemental function celsius_to_fahrenheit(c) result(f)
       implicit none
       real, intent(in) :: c
       real :: f
       f = c * 9.0 / 5.0 + 32.0
   end function

   ! 直接传入数组
   real :: temps(5) = [0.0, 10.0, 20.0, 30.0, 40.0]
   write(*, *) celsius_to_fahrenheit(temps)
   ! 输出: 32.0 50.0 68.0 86.0 104.0

可选参数与关键字参数
=====================

.. code-block:: fortran

   subroutine write_log(msg, level, unit)
       implicit none
       character(*), intent(in)           :: msg
       integer, intent(in), optional      :: level
       integer, intent(in), optional      :: unit

       integer :: lvl, u

       lvl = 0
       if (present(level)) lvl = level
       u = 6   ! 标准输出
       if (present(unit)) u = unit

       write(u, '(A, I2)') trim(msg), lvl
   end subroutine

   ! 调用（可选参数可省略）
   call write_log("信息")                  ! 全部使用默认值
   call write_log("警告", level=1)         ! 指定关键参数
   call write_log("错误", level=2, unit=0) ! 指定多个

.. tip::

   使用 ``present()`` 检查可选参数是否被传入。
   关键字参数使调用更清晰，避免记错参数顺序。

interface 关键字
=================

用于声明外部过程或操作符的重载：

.. code-block:: fortran

   interface
       subroutine external_sub(x, y)
           import :: dp          ! 导入主调模块的 dp 类型
           real(dp), intent(in)  :: x
           real(dp), intent(out) :: y
       end subroutine
   end interface

   ! 操作符重载
   interface operator(+)
       procedure add_vectors
   end interface
