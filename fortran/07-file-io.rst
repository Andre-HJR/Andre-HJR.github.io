==============
文件输入输出
==============

Fortran 拥有丰富且易用的文件 I/O 能力，尤其擅长处理格式化表格数据。

文件操作基础
=============

核心步骤
---------

.. code-block:: text

   1. 打开文件（open 语句） → 分配通道号
   2. 读写文件（read / write 语句）
   3. 关闭文件（close 语句）

.. code-block:: fortran

   program file_demo
       implicit none
       integer :: unit

       ! 打开文件
       open(newunit=unit, file="data.txt", status="unknown")

       ! 读写操作...

       ! 关闭文件
       close(unit)
   end program

文件通道号
-----------

.. code-block:: fortran

   integer :: unit

   ! 方式 1：手动指定（传统）
   open(unit=10, file="data.txt")

   ! 方式 2：newunit（Fortran 2003+，推荐）
   ! 自动分配未使用的通道号
   open(newunit=unit, file="data.txt")
   write(*, *) "通道号:", unit
   close(unit)

.. tip::

   始终使用 ``newunit``——它自动选择可用的通道号，避免冲突。

open 语句
==========

.. code-block:: fortran

   ! 基本格式
   open(unit, file, status, action, position, iostat, errmsg)

   ! 实际示例
   integer :: unit, ios
   character(256) :: err_msg

   open(newunit=unit, file="result.dat", status="replace", &
        action="write", iostat=ios, iomsg=err_msg)
   if (ios /= 0) then
       write(*, *) "打开失败:", trim(err_msg)
       stop
   end if

status 参数
------------

.. list-table::
   :header-rows: 1

   * - status
     - 含义
     - 文件存在
     - 文件不存在
   * - ``"old"``
     - 读取已有文件
     - 正常打开
     - 出错
   * - ``"new"``
     - 创建新文件
     - 出错
     - 创建
   * - ``"replace"``
     - 替换已有文件
     - 清空重新创建
     - 创建新文件
   * - ``"scratch"``
     - 临时文件
     - （关闭时自动删除）
     - 创建临时文件
   * - ``"unknown"``
     - 由编译器决定
     - 正常打开
     - 创建

文本文件读写
=============

写入
-----

.. code-block:: fortran

   integer :: i, unit
   real :: x

   open(newunit=unit, file="output.txt", status="replace", action="write")

   ! 自由格式写入
   write(unit, *) "Hello, Fortran!", 42, 3.14

   ! 格式化写入
   do i = 1, 10
       x = i * 1.5
       write(unit, '(I5, F10.3)') i, x
   end do

   close(unit)

   ! 标准输出：unit = 6 或使用 *
   write(*, *) "输出到屏幕"        ! * 表示标准输出
   write(6, *) "同上"

读取
-----

.. code-block:: fortran

   integer :: i, unit, ios
   real :: x

   open(newunit=unit, file="input.txt", status="old", action="read")

   ! 逐行读取
   do
       read(unit, *, iostat=ios) i, x
       if (ios /= 0) exit          ! 文件结束或出错
       write(*, *) i, x
   end do

   close(unit)

.. caution::

   ``read(..., iostat=ios)`` 检查 I/O 状态：
   - ``ios == 0``：正常
   - ``ios < 0``：文件结束 (EOF)
   - ``ios > 0``：发生错误

格式化描述符
=============

.. code-block:: fortran

   integer :: i = 42
   real(8) :: x = 3.14159265358979_dp
   character(10) :: name = "Fortran"

   ! 各格式示例
   write(*, '(I5)')     i            !   "   42" (整数, 宽 5)
   write(*, '(I0)')     i            !   "42" (I0 = 紧凑输出)
   write(*, '(F8.3)')   real(i)      !   "  42.000" (浮点, 总宽8, 小数3)
   write(*, '(E12.4)')  x            !   "0.3142E+01" (科学计数)
   write(*, '(ES12.4)') x            !   "  3.1416E+00" (现代科学计数)
   write(*, '(A)')      name         !   "Fortran"
   write(*, '(A10)')    name         !   "   Fortran" (右对齐, 宽10)
   write(*, '(L1)')     .true.       !   "T"
   write(*, '(1X, A)')  name         !   前导空格

   ! 组合格式
   write(*, '(A, I0, A, F8.3)') "i=", i, ", x=", x

.. list-table::
   :header-rows: 1

   * - 描述符
     - 含义
     - 示例输出
   * - ``Iw``
     - 整数，宽度 w
     - ``I5`` → ``   42``
   * - ``I0``
     - 整数，紧凑格式
     - ``I0`` → ``42``
   * - ``Fw.d``
     - 浮点，总宽 w，小数 d 位
     - ``F8.3`` → ``  42.000``
   * - ``Ew.d``
     - 科学计数
     - ``E12.4`` → ``0.4210E+02``
   * - ``ESw.d``
     - 现代科学计数（推荐）
     - ``ES12.4`` → ``  4.2000E+01``
   * - ``Aw``
     - 字符串
     - ``A10`` → ``   Fortran``
   * - ``Lw``
     - 逻辑值（T/F）
     - ``L1`` → ``T``
   * - ``wX``
     - 空格
     - ``3X`` → ``   ``
   * - ``/``
     - 换行
     - ``/`` → 新行

namelist（命名列表）
======================

Fortran 特有的强大功能：按名称读写变量组。

.. code-block:: fortran

   program namelist_demo
       implicit none
       integer :: n = 100
       real    :: tol = 1e-6_dp
       logical :: debug = .false.
       character(50) :: method = "RK4"

       ! 定义 namelist
       namelist /params/ n, tol, debug, method

       ! 写入
       open(10, file="params.nml", status="replace")
       write(10, nml=params)
       close(10)

       ! 读取（可修改部分参数，不要求全部提供）
       n = 0; debug = .true.
       open(10, file="params.nml", status="old")
       read(10, nml=params)
       close(10)

       write(*, *) n, tol, debug, method
   end program

生成的 ``params.nml`` 文件内容：

.. code-block:: text

   &PARAMS
     N=100,
     TOL=0.100000000000000E-005,
     DEBUG=F,
     METHOD="RK4",
   /

.. tip::

   Namelist 非常适合**配置文件**：
   - 可读性强，易于手工编辑
   - 可选择性修改部分参数
   - 无需关心格式描述符

二进制文件
===========

.. code-block:: fortran

   integer, parameter :: n = 100
   real(8) :: data(n)
   integer :: unit

   ! 初始化
   data = [ (i * 1.0_dp, i = 1, n) ]

   ! 写二进制文件
   open(newunit=unit, file="data.bin", &
        form="unformatted", status="replace", &
        access="stream")               ! 流式访问（推荐）
   write(unit) data
   close(unit)

   ! 读取
   open(newunit=unit, file="data.bin", &
        form="unformatted", status="old", access="stream")
   read(unit) data
   close(unit)

.. note::

   ``access="stream"`` (Fortran 2003+) 以字节流方式读写，与 C 兼容。
   传统方式使用 ``access="sequential"`` 或 ``access="direct"``。

文件操作常用函数
=================

.. code-block:: fortran

   integer :: ios, unit
   logical :: exists

   ! 检查文件是否存在
   inquire(file="data.txt", exist=exists)

   ! 检查文件是否已打开
   inquire(file="data.txt", opened=exists, number=unit)

   ! 获取文件大小
   inquire(file="data.txt", size=ios)

   ! 检查读写状态
   inquire(unit=10, iostat=ios)

   ! 删除文件
   open(unit=10, file="temp.txt")   ! 先打开
   close(10, status="delete")        ! 关闭时删除
