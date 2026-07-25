==============
函数式编程
==============

Scala 的函数式编程特性使其能够编写简洁、可组合且线程安全的代码。

函数定义
=========

方法定义
---------

.. code-block:: scala

   // 基本函数
   def add(x: Int, y: Int): Int = x + y

   // 单表达式函数可省略大括号
   def square(x: Int): Int = x * x

   // 多行函数体
   def greet(name: String): String =
     val msg = s"Hello, $name"
     msg.toUpperCase

   // 无参数函数
   def now(): Long = System.currentTimeMillis()
   def pi: Double = 3.14159        // 省略 () 表示每次调用重新求值

过程（无返回值）
----------------

.. code-block:: scala

   def log(msg: String): Unit = println(s"[LOG] $msg")
   // 或省略返回类型声明
   def log(msg: String): Unit = println(s"[LOG] $msg")

高阶函数
=========

高阶函数是**接受函数作为参数**或**返回函数**的函数：

.. code-block:: scala

   // 接受函数参数
   def operate(a: Int, b: Int, f: (Int, Int) => Int): Int = f(a, b)

   val sum    = operate(3, 4, _ + _)       // 7
   val product = operate(3, 4, _ * _)      // 12
   val diff   = operate(3, 4, (x, y) => x - y)  // -1

.. code-block:: scala

   // 返回函数（闭包）
   def multiplier(factor: Int): Int => Int =
     (x: Int) => x * factor

   val double = multiplier(2)
   val triple = multiplier(3)
   println(double(5))    // 10
   println(triple(5))    // 15

匿名函数（Lambda）
-------------------

.. code-block:: scala

   // 完整写法
   val f1: (Int, Int) => Int = (x: Int, y: Int) => x + y

   // 类型推断简写
   val f2 = (x: Int, y: Int) => x + y

   // 占位符语法（每个参数只出现一次时）
   val f3: (Int, Int) => Int = _ + _

.. warning::

   占位符语法简洁但易读性下降。建议在简单表达式中使用，复杂逻辑使用显式参数名。

纯函数与副作用
===============

**纯函数** 满足：
1. 相同的输入始终产生相同的输出
2. 不产生副作用（I/O、修改状态、抛出异常）

.. code-block:: scala

   // 纯函数
   def pureAdd(a: Int, b: Int): Int = a + b

   // 不纯函数（依赖外部状态）
   var counter = 0
   def impureAdd(x: Int): Int =
     counter += 1
     x + counter

.. important::

   纯函数易于测试、推理和并行化。尽量将副作用推到程序边界。

不可变性
=========

不可变对象没有可变状态，天然线程安全：

.. code-block:: scala

   // 可变（不推荐）
   import scala.collection.mutable
   val buf = mutable.ListBuffer(1, 2, 3)
   buf += 4

   // 不可变（推荐）
   val list = List(1, 2, 3)
   val newList = list :+ 4    // 返回新 List，原 List 不变

Option、Either 与 Try
======================

Option
-------

表示可能存在或不存在的值，替代 ``null``：

.. code-block:: scala

   def findUser(id: Int): Option[String] =
     if id == 1 then Some("Alice")
     else None

   // 安全访问
   findUser(1).getOrElse("Unknown")      // "Alice"
   findUser(99).getOrElse("Unknown")     // "Unknown"

   // 映射
   findUser(1).map(_.toUpperCase)        // Some("ALICE")
   findUser(99).map(_.toUpperCase)       // None

   // 模式匹配
   findUser(1) match
     case Some(name) => println(s"找到: $name")
     case None       => println("未找到")

Either
-------

表示两个可能的结果：成功（Right）或失败（Left）：

.. code-block:: scala

   def divide(a: Int, b: Int): Either[String, Int] =
     if b == 0 then Left("除数不能为零")
     else Right(a / b)

   divide(10, 2) match
     case Right(result) => println(s"结果: $result")
     case Left(error)   => println(s"错误: $error")

   // 链式调用（Right-biased）
   divide(10, 2).map(_ * 2)   // Right(10)

Try
----

专门用于捕获异常的函数式构造：

.. code-block:: scala

   import scala.util.{Try, Success, Failure}

   def riskyDivide(a: Int, b: Int): Try[Int] = Try(a / b)

   riskyDivide(10, 2) match
     case Success(v) => println(s"成功: $v")
     case Failure(e) => println(s"失败: ${e.getMessage}")

   // 链式操作
   riskyDivide(10, 2)
     .map(_ * 2)
     .getOrElse(0)

Monad 基础概念
===============

Monad 是函数式编程中的核心设计模式，Scala 中的 ``Option``、``List``、``Either``、``Try``
等都是 Monad。一个 Monad 必须满足：

#. **unit** (也称 ``pure`` / ``apply``)：将一个值包装到 Monad 上下文中
#. **flatMap** (也称 ``bind`` / ``>>=``)：将 Monad 中的值通过函数转换，保持上下文

.. code-block:: scala

   // flatMap 链式操作（避免层层嵌套）
   def parseInt(s: String): Option[Int] =
     s.toIntOption

   def sqrt(i: Int): Option[Double] =
     if i >= 0 then Some(math.sqrt(i.toDouble)) else None

   val result = parseInt("16")
     .flatMap(sqrt)

   // for 推导式（Monad 的语法糖）
   val result2 = for
     i <- parseInt("16")
     r <- sqrt(i)
   yield r
   // result2: Some(4.0)

.. tip::

   ``for`` 推导式本质是 ``flatMap`` + ``map`` 的语法糖。
   学习识别模式：每个 ``<-`` 对应一次 ``flatMap``，最后的 ``yield`` 对应 ``map``。
