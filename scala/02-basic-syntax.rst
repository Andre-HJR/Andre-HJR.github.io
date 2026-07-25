===========
基础语法
===========

变量与数据类型
===============

变量定义
---------

.. code-block:: scala

   val name: String = "Scala"    // 不可变（推荐）
   var count: Int = 0            // 可变
   lazy val data = loadFile()    // 惰性求值，首次访问时计算

.. tip::

   **优先使用** ``val``，仅在必须改变引用时才用 ``var``。
   不可变性是函数式编程的基石。

类型推断
---------

Scala 拥有强大的类型推断系统，多数情况可省略类型标注：

.. code-block:: scala

   val msg    = "Hello"          // 推断为 String
   val length = msg.length       // 推断为 Int
   val pi     = 3.14159          // 推断为 Double

基本数据类型
-------------

Scala 中的一切皆对象——基础类型也是对象，拥有方法：

+------------+---------------------------+----------------------------+
| 类型       | 描述                      | 示例                       |
+============+===========================+============================+
| ``Byte``   | 8 位有符号整数            | ``val b: Byte = 127``      |
+------------+---------------------------+----------------------------+
| ``Short``  | 16 位有符号整数           | ``val s: Short = 32767``   |
+------------+---------------------------+----------------------------+
| ``Int``    | 32 位有符号整数           | ``val i = 42``             |
+------------+---------------------------+----------------------------+
| ``Long``   | 64 位有符号整数           | ``val l = 42L``            |
+------------+---------------------------+----------------------------+
| ``Float``  | 32 位浮点数               | ``val f = 3.14f``          |
+------------+---------------------------+----------------------------+
| ``Double`` | 64 位浮点数               | ``val d = 3.14``           |
+------------+---------------------------+----------------------------+
| ``Char``   | 16 位 Unicode 字符        | ``val c = 'A'``            |
+------------+---------------------------+----------------------------+
| ``String`` | 字符串（java.lang.String）| ``val s = "Hello"``        |
+------------+---------------------------+----------------------------+
| ``Boolean``| 布尔值                    | ``val b = true``           |
+------------+---------------------------+----------------------------+

字符串插值
-----------

.. code-block:: scala

   val name   = "Scala"
   val age    = 20
   val msg    = s"$name 诞生于 $age 年前"     // s 插值器
   val fmt    = f"圆周率: $pi%.2f"             // f 插值器（格式化）
   val rawStr = raw"换行符是 \n"               // raw 插值器（不转义）

控制结构
=========

条件表达式
----------

Scala 的 ``if`` 是**表达式**（有返回值）：

.. code-block:: scala

   val score  = 85
   val grade  = if score >= 90 then "A"
                else if score >= 80 then "B"
                else "C"
   // grade: String = "B"

.. note::

   Scala 3 引入了 ``if ... then ... else`` 语法以去除括号。
   传统 ``if (...) {...}`` 风格同样可用。

循环
-----

.. code-block:: scala

   // for 推导式（推荐）
   for i <- 1 to 5 do
     println(i)

   // 带过滤器
   for
     i <- 1 to 10
     if i % 2 == 0
   do
     println(s"偶数: $i")

   // while 循环（尽量少用）
   var i = 0
   while i < 5 do
     println(i)
     i += 1

for 推导式（yield）
--------------------

使用 ``yield`` 从循环中生成新集合：

.. code-block:: scala

   val doubled = for i <- 1 to 5 yield i * 2
   // doubled: IndexedSeq[Int] = Vector(2, 4, 6, 8, 10)

模式匹配
=========

模式匹配是 Scala 最强大的特性之一，替代了传统的 ``switch``：

基本用法
---------

.. code-block:: scala

   def describe(x: Any): String = x match
     case 1          => "数字 1"
     case "hello"    => "问候语"
     case i: Int     => s"整数: $i"
     case s: String  => s"字符串: $s"
     case _          => "未知类型"

.. code-block::
   :caption: 执行结果

   describe(1)          // "数字 1"
   describe("hello")    // "问候语"
   describe(42)         // "整数: 42"

守卫条件
---------

.. code-block:: scala

   def classify(n: Int): String = n match
     case x if x < 0 => "负数"
     case 0          => "零"
     case x if x > 0 => "正数"

解构匹配
---------

.. code-block:: scala

   val pair = (1, "one")
   pair match
     case (a, b) => println(s"$a -> $b")

   case class Person(name: String, age: Int)
   val p = Person("Alice", 30)
   p match
     case Person(n, a) => println(s"$n 今年 $a 岁")
     case Person("Bob", _) => println("发现 Bob")

异常处理
=========

.. code-block:: scala

   try
     val result = riskyOperation()
     println(result)
   catch
     case e: IllegalArgumentException => println(s"参数错误: $e")
     case e: Exception                => println(s"其他异常: $e")
   finally
     cleanup()   // 一定会执行

.. tip::

   Scala 推崇用 ``Try`` / ``Either`` / ``Option`` 替代裸异常，
   见函数式编程章节。
