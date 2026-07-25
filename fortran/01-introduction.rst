==========================================
Fortran 简介与 IVF 2011 环境搭建
==========================================

Fortran 语言概述
=================

Fortran（**For**\ mula **Tran**\ slation）诞生于 1957 年，由 IBM 的 John Backus 领导开发，
是历史上第一个高级编程语言，至今仍是科学计算和数值分析领域的核心工具。

核心特点
---------

- **数值计算王者**：原生数组运算、复数、高精度浮点
- **性能卓越**：编译器优化成熟，与 C/C++ 性能相当
- **科学计算生态**：完善的数学库（LAPACK、BLAS、FFTW）
- **数组为中心**：Fortran 90+ 的数组操作语法极其简洁
- **Fortran 90+ 现代化**：模块、派生类型、接口、指针

Intel Visual Fortran 2011
============================

IVF 2011（Intel Visual Fortran Composer XE 2011）是 Intel 推出的 Fortran 编译器，
集成在 Intel Parallel Studio XE 2011 中，与 Visual Studio 2010 深度整合。

主要特性：

- 完整支持 Fortran 90/95，大部分 Fortran 2003/2008
- 集成 Visual Studio 2010 IDE
- 自动向量化与并行化优化
- 包含 Intel Math Kernel Library (MKL)
- OpenMP 3.0 并行支持
- 兼容 Co-Array Fortran

.. note::

   Fortran **版本演进**：

   .. list-table::
      :header-rows: 1

      * - 版本
        - 主要特性
        - IVF 支持
      * - Fortran77
        - 固定格式、COMMON、DATA
        - 完整
      * - Fortran90
        - 自由格式、模块、派生类型
        - 完整
      * - Fortran95
        - FORALL、PURE 过程
        - 完整
      * - Fortran2003
        - OOP、C 互操作、I/O 增强
        - 大部分
      * - Fortran2008
        - Co-Array、子模块
        - 部分

环境搭建
=========

安装 Intel Parallel Studio XE 2011
------------------------------------

.. code-block:: text

   1. 下载 Intel Parallel Studio XE 2011 安装包
   2. 运行安装程序，选择 "Intel Visual Fortran Composer XE"
   3. 选择集成到 Visual Studio 2010
   4. 安装 Intel MKL（建议勾选完整安装）
   5. 配置环境变量:
      - IFORT_COMPILER11 = C:\Program Files\Intel\Composer XE 2011
      - 添加 %IFORT_COMPILER11%\bin\intel64 到 PATH

验证安装：

.. code-block:: text

   ifort /?
   ifort /version

创建第一个 Fortran 项目
-------------------------

.. code-block:: text

   1. 启动 Visual Studio 2010
   2. 文件 → 新建 → 项目
   3. 选择 Intel Visual Fortran → Console Application
   4. 项目名称: hello_fortran
   5. 选择 "Empty Project"
   6. 在 Solution Explorer 中右键 Source Files → Add → New Item
   7. 选择 Fortran Free-Format File (.f90)

第一个程序
-----------

.. code-block:: fortran
   :caption: main.f90

   program hello
       implicit none
       write(*, *) "Hello, Fortran!"
   end program hello

编译与运行：

.. code-block:: text

   调试 → 开始执行(不调试)   (Ctrl + F5)
   或
   调试 → 开始调试           (F5)

文件扩展名约定
===============

.. list-table::
   :header-rows: 1

   * - 扩展名
     - 格式
     - 说明
   * - ``.f`` / ``.for``
     - 固定格式
     - 传统 Fortran 77 风格（72 列限制）
   * - ``.f90`` / ``.f95``
     - 自由格式
     - Fortran 90/95 现代风格（推荐）
   * - ``.f03`` / ``.f08``
     - 自由格式
     - Fortran 2003/2008 标准
   * - ``.i``
     - 固定格式
     - 包含文件（类似 C 的 .h）
   * - ``.inc``
     - 固定/自由
     - 包含文件（通用）

.. important::

   **自由格式** (Free-Form) 是现代 Fortran 的标准写法：
   - 无列限制（最多 132 列）
   - ``!`` 开头为注释
   - ``&`` 续行
   - 不区分大小写

启动脚本与环境变量
===================

IVF 2011 需要设置编译器环境：

.. code-block:: text

   "C:\Program Files\Intel\Composer XE 2011\bin\compilervars.bat" intel64

或在 Visual Studio 命令提示符下：

.. code-block:: text

   开始 → Intel Parallel Studio XE 2011 → Compiler XE 2011
   → Intel 64 Visual Studio 2010 模式

.. tip::

   如果使用命令行编译：

   .. code-block:: text

      ifort /nologo /standard-semantics /check:all /warn:all main.f90
