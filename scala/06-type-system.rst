==========
类型系统
==========

Scala 拥有强大的静态类型系统，能够在不牺牲表达能力的前提下提供类型安全。

泛型
=====

泛型允许定义适用于多种类型的类、特质和方法：

.. code-block:: scala

   // 泛型类
   class Box[A](value: A):
     def get: A = value

   val intBox  = Box(42)
   val strBox  = Box("Hello")

.. code-block:: scala

   // 泛型方法
   def identity[A](x: A): A = x
   identity(42)        // 推断为 Int
   identity("Hello")   // 推断为 String

泛型约束
---------

**上界（Upper Bound）**：限定类型必须是某个类型的子类型：

.. code-block:: scala

   // A 必须是 Ordered[A] 的子类型
   def max[A <: Ordered[A]](a: A, b: A): A = if a >= b then a else b

**下界（Lower Bound）**：限定类型必须是某个类型的父类型：

.. code-block:: scala

   // B 必须是 A 的父类型
   def coerce[A, B >: A](a: A): B = a

上下界可以结合使用：

.. code-block:: scala

   // [A >: Low <: High]

协变、逆变与不变
=================

+----------+--------------+----------------------------------------+----------------------------------+
| 修饰符   | 名称         | 含义                                   | 示例                             |
+==========+==============+========================================+==================================+
| ``+T``   | 协变         | ``Container[Parent]`` 可引用           | ``List[+T]``, ``Option[+T]``     |
|          |              | ``Container[Child]``                   |                                  |
+----------+--------------+----------------------------------------+----------------------------------+
| ``-T``   | 逆变         | ``Container[Child]`` 可引用            | ``Function1[-T, +R]`` 中参数类型 |
|          |              | ``Container[Parent]``                  |                                  |
+----------+--------------+----------------------------------------+----------------------------------+
| ``T``    | 不变         | 只能持有精确类型                       | ``Array[T]``, ``mutable.Set[T]`` |
+----------+--------------+----------------------------------------+----------------------------------+

.. code-block:: scala

   class Animal
   class Dog extends Animal

   // 协变
   val dogs: List[Dog] = List(Dog())
   val animals: List[Animal] = dogs      // OK

   // 逆变：函数参数是逆变的
   val f: Dog => Dog = identity
   val g: Animal => Dog = f              // OK（Animal => Dog 可替代 Dog => Dog）

示例：自定义协变容器：

.. code-block:: scala

   sealed trait MyList[+A]:
     def head: A
     def tail: MyList[A]
     def ::[B >: A](elem: B): MyList[B] = MyCons(elem, this)

   case object MyNil extends MyList[Nothing]
   case class MyCons[A](head: A, tail: MyList[A]) extends MyList[A]

类型约束
=========

上下文界定（Context Bound）
----------------------------

.. code-block:: scala

   // 要求存在 implicit/given Ordering[A]
   def sort[A: Ordering](list: List[A]): List[A] =
     list.sorted

视图界定（View Bound，Scala 2 风格，Scala 3 中移除）
------------------------------------------------------

.. code-block:: scala

   // Scala 2 写法（已废弃）
   // def max[A <% Ordered[A]](a: A, b: A): A = ...

   // Scala 3 使用 given/using 替代
   def max[A](a: A, b: A)(using ord: Ordering[A]): A =
     if ord.gteq(a, b) then a else b

给定实例与 using 子句（Scala 3）
==================================

Scala 3 用 ``given`` / ``using`` 替代了 Scala 2 的 ``implicit``：

.. code-block:: scala

   // 定义给定实例
   given intOrdering: Ordering[Int] with
     def compare(x: Int, y: Int): Int = x - y

   // 使用 given 实例的 using 子句
   def maxList[A](list: List[A])(using ord: Ordering[A]): A =
     list.max

   // 调用时无需传入 using 参数（编译期自动解析）
   maxList(List(3, 1, 4, 1, 5))    // 5

上下文参数简写：

.. code-block:: scala

   def maxList[A: Ordering](list: List[A]): A =
     // summon[Ordering[A]] 获取 given 实例
     list.max

类型类模式
-----------

类型类（Type Class）是 Scala 中的经典设计模式，结合了泛型和 given/using：

.. code-block:: scala

   // 1. 定义类型类的 trait
   trait Show[A]:
     def show(a: A): String

   // 2. 提供给定实例
   given Show[Int] with
     def show(a: Int): String = a.toString

   given Show[String] with
     def show(a: String): String = s""""$a""""

   // 3. 使用类型类
   def printShow[A](a: A)(using s: Show[A]): Unit =
     println(s.show(a))

   printShow(42)           // 42
   printShow("Hello")      // "Hello"

联合类型与交集类型（Scala 3）
==============================

联合类型（Union Type）
-----------------------

表示值可以是多种类型之一：

.. code-block:: scala

   type Result = Int | String

   def handle(result: Result): String = result match
     case i: Int    => s"整数: $i"
     case s: String => s"字符串: $s"

交集类型（Intersection Type）
-----------------------------

表示值必须同时满足多种类型：

.. code-block:: scala

   trait Readable:
     def read: String

   trait Writable:
     def write(data: String): Unit

   def process(rw: Readable & Writable): Unit =
     val data = rw.read
     rw.write(data)

不透明类型（Opaque Type）
==========================

不透明类型为现有类型提供类型安全的包装，运行时无开销：

.. code-block:: scala

   object Domain:
     opaque type UserId = Long
     object UserId:
       def apply(id: Long): UserId = id
       extension (id: UserId) def value: Long = id

   import Domain.UserId
   val uid: UserId = UserId(42)
   // val bad: Long = uid   // 编译错误！不能在不透明类型定义域外解包
