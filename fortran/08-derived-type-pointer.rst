========================
派生类型与指针
========================

Fortran 90+ 引入了派生类型（类似 C 的 struct）和指针（类似 C 的指针但更安全）。

派生类型（Derived Type）
==========================

定义与使用
-----------

.. code-block:: fortran

   ! 定义派生类型
   type :: Person
       character(50) :: name
       integer       :: age
       real          :: height     ! 身高（米）
       logical       :: employed
   end type Person

   ! 声明变量
   type(Person) :: alice, bob

   ! 成员赋值
   alice%name = "Alice"
   alice%age  = 30
   alice%height = 1.68
   alice%employed = .true.

   ! 结构体构造器
   bob = Person("Bob", 25, 1.75, .false.)

   ! 整体赋值
   alice = bob        ! 复制所有成员

.. note::

   使用 ``%`` 访问成员（不是 C 的 ``.`` 或 ``->``）。

嵌套派生类型
-------------

.. code-block:: fortran

   type :: Date
       integer :: year
       integer :: month
       integer :: day
   end type

   type :: Employee
       integer      :: id
       character(50) :: name
       type(Date)   :: hire_date     ! 嵌套
       type(Date)   :: birth_date
       logical      :: active
   end type

   ! 嵌套初始化
   type(Employee) :: emp
   emp = Employee(id=2001, &
                  name="Eve", &
                  hire_date=Date(2020, 3, 15), &
                  birth_date=Date(1990, 7, 22), &
                  active=.true.)

   ! 访问嵌套成员
   write(*, *) "入职年份:", emp%hire_date%year

派生类型数组
-------------

.. code-block:: fortran

   type(Person) :: people(100)
   type(Person), allocatable :: team(:)

   ! 初始化
   people(1) = Person("Alice", 30, 1.68, .true.)
   people(2) = Person("Bob", 25, 1.75, .false.)

   ! 分配动态数组
   allocate(team(50))
   team(1) = Person("Charlie", 35, 1.80, .true.)

   ! 数组遍历
   integer :: i
   do i = 1, 2
       write(*, *) trim(people(i)%name), people(i)%age
   end do

派生类型过程绑定（Fortran 2003+ OOP）
---------------------------------------

.. code-block:: fortran

   type :: Vector
       real :: x, y, z
   contains
       procedure :: length   => vector_length
       procedure :: normalize => vector_normalize
   end type

   contains
       real function vector_length(this)
           class(Vector), intent(in) :: this
           vector_length = sqrt(this%x**2 + this%y**2 + this%z**2)
       end function

       subroutine vector_normalize(this)
           class(Vector), intent(inout) :: this
           real :: len
           len = this%length()
           if (len > 0) then
               this%x = this%x / len
               this%y = this%y / len
               this%z = this%z / len
           end if
       end subroutine

   ! 使用
   type(Vector) :: v
   v = Vector(1.0, 2.0, 3.0)
   write(*, *) v%length()        ! 通过 % 调用绑定过程
   call v%normalize()

可分配组件
-----------

.. code-block:: fortran

   type :: DynamicData
       integer :: n
       real, allocatable :: values(:)  ! 可分配数组组件
   end type

   type(DynamicData) :: d

   d%n = 100
   allocate(d%values(d%n))
   d%values = [ (i * 1.0, i = 1, d%n) ]

   ! 可在声明时自动分配（Fortran 2003+）
   type(DynamicData) :: d2 = DynamicData(50, [ (i*1.0, i=1,50) ])

   ! 自动释放
   deallocate(d%values)

Fortran 指针
=============

Fortran 指针是**类型安全**的别名，不同于 C 指针（不是地址运算）。

基本用法
---------

.. code-block:: fortran

   integer, target :: x = 42       ! target: 可被指针指向
   integer, pointer :: p           ! 指针声明

   p => x                          ! 指针指向 x（不是地址运算）
   write(*, *) p                   ! 42

   p = 100                         ! 通过指针修改
   write(*, *) x                   ! 100（x 被修改！）

.. important::

   ``p => x`` 是**绑定**（pointer association），不是取地址！
   Fortran 指针永远指向一个有 ``target`` 属性的变量。

指针与数组
-----------

.. code-block:: fortran

   integer, target :: arr(10)
   integer, pointer :: p(:)

   ! 指向整个数组
   p => arr
   p = [ (i, i = 1, 10) ]

   ! 指向数组切片
   p => arr(1:5)
   p => arr(1:10:2)       ! 步长切片 [1,3,5,7,9]

   ! 动态分配指针（不依赖 target）
   allocate(p(100))
   p = 0
   deallocate(p)

可分配 vs 指针
---------------

.. list-table::
   :header-rows: 1

   * - 特性
     - ``allocatable``
     - ``pointer``
   * - 内存管理
     - 自动自动释放
     - 需手动 ``deallocate``
   * - 别名能力
     - 不能
     - 可以指向 target
   * - 数组切片指向
     - 不能
     - 可以
   * - 性能
     - 更好（编译器优化）
     - 有额外开销
   * - 推荐度
     - **优先使用**
     - 仅在需要别名时使用

.. tip::

   在 Fortran 中，能用 ``allocatable`` 的地方**不要用 pointer**——
   可分配数组更安全、更高效。

链表示例
=========

.. code-block:: fortran
   :caption: 简单的链表实现

   module linked_list
       implicit none
       private

       type, public :: node
           integer :: value
           type(node), pointer :: next => null()
       end type

   contains
       subroutine append(head, val)
           type(node), pointer, intent(inout) :: head
           integer, intent(in) :: val
           type(node), pointer :: curr

           if (associated(head)) then
               curr => head
               do while (associated(curr%next))
                   curr => curr%next
               end do
               allocate(curr%next)
               curr%next%value = val
               curr%next%next => null()
           else
               allocate(head)
               head%value = val
               head%next => null()
           end if
       end subroutine

       subroutine print_list(head)
           type(node), pointer, intent(in) :: head
           type(node), pointer :: curr
           curr => head
           do while (associated(curr))
               write(*, '(I0, " -> ")', advance="no") curr%value
               curr => curr%next
           end do
           write(*, *) "null"
       end subroutine
   end module

.. caution::

   Fortran 的指针操作比 C 更受限：
   - 无地址运算（``ptr + 1`` 不允许）
   - 无指针类型转换
   - ``associated()`` 检查指针是否被绑定
   - 指针只能重定向（不能修改地址关系外的数据）
