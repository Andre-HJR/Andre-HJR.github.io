===================
模块与作用域
===================

模块（module）是 Fortran 90 最重要的现代化特性，用于封装数据、类型和过程。

模块基础
=========

基本结构
---------

.. code-block:: fortran
   :caption: math_utils.f90

   module math_utils
       implicit none
       private                     ! 默认私有
       public :: pi, add, factorial ! 显式公开接口

       real, parameter :: pi = 3.14159265

   contains
       function add(a, b) result(r)
           implicit none
           integer, intent(in) :: a, b
           integer :: r
           r = a + b
       end function add

       recursive function factorial(n) result(r)
           implicit none
           integer, intent(in) :: n
           integer :: r

           if (n <= 1) then
               r = 1
           else
               r = n * factorial(n - 1)
           end if
       end function factorial

       ! 私有过程：模块内部使用
       function internal_helper(x) result(y)
           real, intent(in) :: x
           real :: y
           y = x + 1.0
       end function

   end module math_utils

.. important::

   ``private`` 声明在模块开头，使所有实体默认私有。
   使用 ``public :: name`` 显式公开需要的接口——这叫**最小暴露原则**。

使用模块
---------

.. code-block:: fortran

   program main
       use math_utils                     ! 引入模块
       implicit none

       ! 可以直接使用模块中的公开实体
       write(*, *) pi                     ! 3.14159265
       write(*, *) add(3, 4)             ! 7
       write(*, *) factorial(5)           ! 120

       ! internal_helper 不可见（private）
       ! write(*, *) internal_helper(1.0)  ! 错误！
   end program main

利用 only 精确控制
--------------------

.. code-block:: fortran

   ! 只引入需要的实体——避免命名冲突
   use math_utils, only: pi, factorial

   ! 重命名引入的实体——解决冲突
   use math_utils, only: compute_pi => pi

   ! 引入全部（不推荐——容易意外覆盖）
   use math_utils

.. note::

   使用 ``only`` 子句是良好的编程习惯：
   - 明确代码依赖了什么
   - 避免命名空间污染
   - 编译器可以更好地优化

模块的可见性控制
=================

.. code-block:: fortran

   module data_module
       implicit none
       private                     ! 全部默认私有

       ! 公共接口
       public :: set_value, get_value, init

       ! 模块级数据（封装，类似 C 的 static）
       integer :: shared_value = 0

   contains
       subroutine init()
           shared_value = 0
       end subroutine

       subroutine set_value(v)
           integer, intent(in) :: v
           shared_value = v
       end subroutine

       function get_value() result(v)
           integer :: v
           v = shared_value
       end function
   end module

.. tip::

   这种封装模式保证了数据只能通过公开接口访问——
   这就是 Fortran 版本的"数据隐藏"（类似 C 的 ``static`` 或 Java 的 ``private``）。

嵌套模块（子模块）
===================

Fortran 2008+ 支持子模块，将大型模块拆分为多文件。

.. code-block:: fortran

   ! 父模块
   module matrix_ops
       implicit none
       private
       public :: matrix_multiply
   contains
       interface
           module subroutine matrix_multiply(a, b, c)
               real, intent(in)  :: a(:,:), b(:,:)
               real, intent(out) :: c(:,:)
           end subroutine
       end interface
   end module

   ! 子模块
   submodule (matrix_ops) matrix_ops_impl
       implicit none
   contains
       module subroutine matrix_multiply(a, b, c)
           ! 实现放在这里，主模块更简洁
           c = matmul(a, b)
       end subroutine
   end submodule

模块间依赖
===========

.. code-block:: fortran
   :caption: 模块间的 use 链

   ! module_a.f90
   module module_a
       implicit none
       real, parameter :: pi = 3.14
   end module

   ! module_b.f90
   module module_b
       use module_a               ! 依赖 module_a
       implicit none
       real, parameter :: two_pi = 2 * pi
   end module

   ! main.f90
   program main
       use module_b               ! 间接使用 module_a
       implicit none
       write(*, *) two_pi
   end program main

.. caution::

   模块间**不能循环依赖**（A use B, B use A）。
   可通过将公共类型提取到单独的"基础模块"来解决。

作用域规则
===========

.. code-block:: fortran

   program scope_demo
       implicit none
       integer :: x = 10     ! 程序级变量

       call inner()

   contains
       subroutine inner()
           integer :: x = 20  ! 局部变量，遮蔽外部 x
           write(*, *) x      ! 20
       end subroutine
   end program

.. list-table::
   :header-rows: 1

   * - 作用域级别
     - 可见性范围
     - 生命周期
   * - 程序/模块级
     - 整个程序/模块
     - 程序运行期
   * - 过程级
     - 过程内部
     - 过程调用期间
   * - 代码块级
     - ``do`` / ``if`` / ``block`` 内
     - 代码块执行期间
   * - ``module`` 变量
     - 模块内（可用 ``public/private`` 控制）
     - 程序运行期
   * - ``save`` 变量
     - 过程内
     - 程序运行期（保留值）

block 结构（Fortran 2008+）
============================

.. code-block:: fortran

   program block_demo
       implicit none
       integer :: x = 10

       block
           integer :: x          ! 块作用域变量
           x = 20
           write(*, *) "块内:", x    ! 20
       end block

       write(*, *) "块外:", x        ! 10
   end program

.. tip::

   ``block`` 结构可以创建临时作用域，避免变量名冲突，
   也便于在需要时才声明临时变量。

模块组织建议
=============

.. code-block::

   项目模块组织示例：

   constants.f90     → 模块: constants（物理常数、参数）
   types.f90         → 模块: types（派生类型定义）
   io.f90            → 模块: io_module（I/O 操作）
   math.f90          → 模块: math_utils（数学函数）
   main.f90          → 程序: main（主程序入口）

.. note::

   IVF 2011 中，模块的编译顺序很重要：先编译被依赖的模块。
   通常在解决方案管理器中按依赖顺序排列文件。
