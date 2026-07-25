============
基础语法
============

Fortran 的语法以科学计算为核心设计，数组运算是其最大的亮点。

程序结构
=========

.. code-block:: fortran
   :caption: 基本程序结构

   program main
       ! 程序主体
       implicit none

       ! 变量声明
       integer :: i
       real    :: x

       ! 执行语句
       i = 10
       x = 3.14
       write(*, *) i, x

   end program main

.. important::

   **总是使用** ``implicit none``！

   如果不使用，Fortran 会隐式声明变量类型：
   - ``i``、``j``、``k``、``l``、``m``、``n`` 开头的变量默认为 ``integer``
   - 其他字母开头的变量默认为 ``real``

   这会导致拼写错误难以发现。``implicit none`` 强制所有变量必须显式声明。

注释
=====

.. code-block:: fortran

   ! 这是单行注释（自由格式）
   ! 每一行都需要以 ! 开头

   ! 没有多行注释，但可以这样：
   !***********************************************************************
   !  这是一个注释块，用星号装饰边框
   !  用途：函数说明
   !***********************************************************************

   i = 1     ! 行尾注释

数据类型
=========

基本数据类型
-------------

.. code-block:: fortran

   ! 整数
   integer            :: i       ! 默认 4 字节
   integer(2)         :: s       ! 2 字节（short）
   integer(4)         :: i4      ! 4 字节（默认）
   integer(8)         :: i8      ! 8 字节（long long）

   ! 实数
   real               :: x       ! 默认 4 字节
   real(4)            :: r4      ! 单精度
   real(8)            :: r8      ! 双精度（常用）
   double precision   :: d       ! 双精度（等同 real(8)）

   ! 复数
   complex            :: c       ! 单精度复数
   complex(8)         :: c8      ! 双精度复数
   double complex     :: z       ! 双精度复数

   ! 逻辑
   logical            :: flag    ! .true. 或 .false.

   ! 字符
   character(len=20)  :: name    ! 定长字符串
   character(50)      :: msg     ! len=50 的简写

.. tip::

   推荐方式：使用 ``selected_real_kind`` 和 ``selected_int_kind``
   以确保可移植性：

   .. code-block:: fortran

      integer, parameter :: dp = selected_real_kind(15, 307)
      real(dp) :: x   ! 双精度

常量定义
---------

.. code-block:: fortran

   ! 命名常量
   real, parameter :: PI = 3.1415926535
   integer, parameter :: MAX_SIZE = 1000

   ! 字符常量
   character(*), parameter :: APP_NAME = "My Fortran App"

   ! 字面常量
   real :: x = 3.14_dp    ! _dp 后缀指定精度
   integer :: y = 100_8   ! 8 字节整数

变量声明
=========

.. code-block:: fortran

   ! 基本声明
   integer :: a
   real    :: x, y, z              ! 同时声明多个
   real    :: arr(10)              ! 数组
   integer :: i = 0                ! 声明时赋初值

   ! 属性修饰
   integer, parameter :: N = 100   ! 常量
   integer, save      :: count = 0 ! 保持值（静态）
   integer, allocatable :: buf(:)  ! 可分配数组
   integer, target    :: ptr       ! 可被指针指向
   integer, pointer   :: p         ! 指针

运算符
=======

算术运算符
-----------

.. code-block:: fortran

   integer :: a = 10, b = 3

   a + b       ! 13  加法
   a - b       ! 7   减法
   a * b       ! 30  乘法
   a / b       ! 3   整数除法（截断）
   mod(a, b)   ! 1   取模（函数形式）
   a ** 2      ! 100 乘方

关系运算符
-----------

.. code-block:: fortran

   a == b      ! 等于
   a /= b      ! 不等于
   a > b       ! 大于
   a < b       ! 小于
   a >= b      ! 大于等于
   a <= b      ! 小于等于

   ! 旧式写法（Fortran 77 风格，仍然可用）
   a .eq. b
   a .ne. b
   a .gt. b

逻辑运算符
-----------

.. code-block:: fortran

   .not. flag       ! 逻辑非
   a .and. b        ! 逻辑与
   a .or. b         ! 逻辑或
   a .eqv. b        ! 逻辑相等
   a .neqv. b       ! 逻辑不等

字符串操作
===========

.. code-block:: fortran

   character(20) :: str1, str2, str3

   str1 = "Hello"
   str2 = "World"

   ! 连接
   str3 = trim(str1) // " " // str2   ! "Hello World"

   ! 子串
   str3(1:5)        ! "Hello"
   str3(7:)         ! "World"

   ! 字符串函数
   len(str3)        ! 20（声明长度）
   len_trim(str3)   ! 11（去除尾部空格）
   index(str3, "o") ! 5（查找位置）
   trim(str1)       ! 去除尾部空格

.. tip::

   Fortran 的字符串是**定长**的，短字符串赋值给长字符串时会用空格填充右侧。
   使用 ``trim()`` 去除尾部空格后再拼接。

格式化输出
===========

.. code-block:: fortran

   integer :: i = 42
   real    :: x = 3.14159
   character(10) :: name = "Fortran"

   ! 自由格式
   write(*, *) "i =", i, "x =", x, "name =", name

   ! 格式化输出
   write(*, '(A, I5)')      "整数: ", i
   write(*, '(A, F8.3)')    "浮点: ", x      ! F8.3 = 总宽8, 小数3位
   write(*, '(A, E12.4)')   "科学: ", x       ! E12.4 = 科学计数
   write(*, '(A, ES12.4)')  "科学(现代): ", x ! ES 更易读

   ! 格式化表格输出
   write(*, '(I5, F10.3, 2X, A)') i, x, name

   ! 读写格式字符串说明
   ! I5   = 整数, 占 5 列
   ! F8.3 = 浮点, 总宽 8, 小数 3 位
   ! E12.4 = 科学计数法
   ! A    = 字符串
   ! X    = 空格
   ! /    = 换行
   ! 2X   = 2 个空格
