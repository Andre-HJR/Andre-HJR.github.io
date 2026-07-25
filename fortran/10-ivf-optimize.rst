============================
IVF 编译器优化与调试
============================

本章介绍 IVF 2011 的编译器选项、性能优化和调试技术。

编译器选项速查
===============

优化选项
---------

.. list-table::
   :header-rows: 1

   * - 选项
     - 说明
     - 推荐场合
   * - ``/Od``
     - 禁用优化
     - 调试阶段
   * - ``/O1``
     - 最小化代码大小
     - 空间受限
   * - ``/O2``
     - 最大化速度（默认）
     - 通用发布版
   * - ``/O3``
     - 更激进优化
     - 科学计算核心
   * - ``/fast``
     - 等同 ``/O3 /Qipo /QxHost``
     - 最高性能
   * - ``/Qipo``
     - 跨文件过程间优化
     - 多文件项目
   * - ``/QxHost``
     - 生成主机最高指令集
     - 专用部署
   * - ``/QaxAVX``
     - 生成多指令集路径
     - 通用部署

.. caution::

   ``/fast`` 中的 ``/QxHost`` 会生成仅当前 CPU 可运行的代码。
   分发程序时使用 ``/QaxAVX`` 生成兼容多平台的可执行文件。

调试选项
---------

.. list-table::
   :header-rows: 1

   * - 选项
     - 说明
   * - ``/Zi``
     - 生成完整调试信息
   * - ``/debug:full``
     - 完整调试符号
   * - ``/check:all``
     - 启用运行时检查（数组越界、未初始化等）
   * - ``/warn:all``
     - 所有警告
   * - ``/traceback``
     - 异常时打印调用栈（发布版也很有用）
   * - ``/fpe:0``
     - 浮点异常捕获（除零、溢出）
   * - ``/Qtrapuv``
     - 检测未初始化变量

.. code-block:: text

   ! 推荐的调试编译选项
   ifort /Od /Zi /debug:full /check:all /warn:all /traceback /fpe:0 /Qtrapuv main.f90

   ! 推荐的发布版编译选项
   ifort /O3 /Qipo /QaxAVX /traceback main.f90

运行时检查
============

.. code-block:: fortran

   ! 编译时使用 /check:all 启用以下检查：

   integer :: arr(10), i, x

   ! 1. 数组越界检查
   i = 15
   x = arr(i)      ! 运行时错误: 下标越界

   ! 2. 未初始化变量检查
   integer :: y    ! 未初始化
   write(*, *) y   ! 警告

   ! 3. 字符串越界
   character(5) :: str
   str = "Hello World"  ! 截断警告

   ! 4. 整数算术
   integer :: a = 2147483647
   a = a + 1              ! 溢出检查（默认不检查）

.. tip::

   发布版建议包含 ``/traceback``，这样用户反馈崩溃时
   可以快速定位到问题行。

性能分析与调优
===============

代码分析
----------

.. code-block:: text

   ! 生成性能分析数据
   ifort /Qprofile /Qprof-gen /O2 myapp.f90
   myapp.exe
   ifort /Qprofile /Qprof-use /O2 myapp.f90

.. code-block:: text

   1. /Qprof-gen: 编译时插入性能计数代码
   2. 运行程序（产生 .dyn 文件）
   3. /Qprof-use: 使用 profile 数据优化编译

向量化报告
-----------

.. code-block:: text

   ! 查看编译器是否向量化了循环
   ifort /O2 /Qvec-report:2 main.f90

   报告级别：
   0 = 不输出
   1 = 报告已向量化的循环
   2 = 报告已向量化和未向量化的循环（含原因）

.. code-block:: text

   常见未向量化原因：
   loop was not vectorized: non-consecutive access
   loop was not vectorized: data dependence
   loop was not vectorized: call inside loop

优化循环的指导原则
--------------------

.. code-block:: fortran

   ! 好的向量化候选：
   do i = 1, n
       a(i) = b(i) + c(i)      ! 连续访问，无依赖
   end do

   ! 不好的例子：
   do i = 1, n
       a(indx(i)) = b(i)        ! 间接寻址（随机访问）
   end do

   do i = 2, n
       a(i) = a(i-1) + b(i)     ! 依赖前一个元素
   end do

.. tip::

   帮助编译器优化的建议：
   - 使用连续内存访问（stride-1 模式）
   - 避免循环内函数调用（用 inline 或纯过程）
   - 使用 ``!dir$ ivdep`` 忽略循环依赖

   .. code-block:: fortran

      !dir$ ivdep             ! 告诉编译器忽略可能的数据依赖
      do i = 1, n
          a(i) = a(i) + b(i)
      end do

调试技术
=========

VS 集成调试器
--------------

.. code-block:: text

   F10         逐过程
   F11         逐语句
   Shift+F11   跳出
   F9          断点
   Ctrl+F5     运行（不调试）

   # 调试时可以查看的变量表达式：
   arr(1:5)         查看数组前 5 个元素
   mat(:, 1)        查看矩阵第一列
   type%member      查看派生类型成员

条件断点
----------

.. code-block:: text

   右键断点 → Condition → 输入条件表达式
   例如: i == 100  （当 i 等于 100 时中断）
          mod(i,10) == 0 （每 10 次中断一次）

IVF 调试环境变量
----------------

.. code-block:: text

   ! 设置浮点异常环境变量
   set MKL_FPI_MODE=strict                    ! MKL 严格模式
   set FLIBCNTL=STANDARD                      ! Libm 标准模式

   ! 调试内存分配
   set MPICH_HOME=C:\Program Files\Intel\MPI  ! MPI 调试

常见错误与解决
===============

.. list-table::
   :header-rows: 1

   * - 错误
     - 原因
     - 解决
   * - ``segmentation fault``
     - 数组越界或指针错误
     - 加 ``/check:all`` 编译排查
   * - ``stack overflow``
     - 栈空间不足（大型局部数组、递归过深）
     - 用 ``allocatable`` 代替自动数组，或调大栈
   * - ``floating point exception``
     - 除零、溢出
     - 加 ``/fpe:0`` 跟踪
   * - ``undefined reference``
     - 缺少库或目标文件
     - 检查链接器设置
   * - ``internal compiler error``
     - 编译器 bug
     - 简化代码、升级编译器补丁
   * - ``forrtl: severe (41)``
     - 文件 I/O 错误
     - 检查文件路径和权限

项目配置对比
==============

.. list-table::
   :header-rows: 1

   * - 配置
     - Debug
     - Release
   * - 优化
     - ``/Od``
     - ``/O3 /Qipo``
   * - 调试信息
     - ``/Zi /debug:full``
     - ``/Zi`` (可选)
   * - 运行检查
     - ``/check:all``
     - 无
   * - 警告
     - ``/warn:all``
     - ``/warn:declarations``
   * - 指令集
     - ``/QxSSE4.2``
     - ``/QaxAVX``
   * - 浮点
     - ``/fpe:0``
     - ``/fp:fast=2``
   * - 链接
     - 调试运行时库
     - 发布运行时库

.. tip::

   将常用编译选项保存为 VS 项目配置模板：
   ``项目 → 导出模板`` —— 下次新项目可直接使用。
