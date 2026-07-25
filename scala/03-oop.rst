================
面向对象编程
================

Scala 是一门纯粹的面向对象语言——**每个值都是对象**，**每个操作都是方法调用**。

类与对象
=========

定义类
-------

.. code-block:: scala

   class Person(name: String, age: Int):
     def greet(): String = s"Hi, I'm $name, $age years old."
     override def toString: String = s"Person($name, $age)"

.. code-block:: scala

   val alice = Person("Alice", 30)
   println(alice.greet())   // Hi, I'm Alice, 30 years old.

.. note::

   Scala 3 使用轻量级的 ``class ... (params):`` 语法。
   类主体直接用缩进开始，无需大括号。

构造函数
---------

Scala 的**主构造函数**直接写在类签名中：

.. code-block:: scala

   // 主构造函数参数 name, age
   class Person(val name: String, val age: Int):
     // 辅助构造函数
     def this(name: String) = this(name, 0)

参数可见性修饰符：

+------------------+------------------------------------------------+
| 修饰符           | 效果                                           |
+==================+================================================+
| 无修饰符         | 私有，不作为字段存储                           |
+------------------+------------------------------------------------+
| ``val``          | 公开的只读字段，生成 getter                    |
+------------------+------------------------------------------------+
| ``var``          | 公开的可读写字段，生成 getter + setter         |
+------------------+------------------------------------------------+
| ``private val``  | 私有的只读字段                                 |
+------------------+------------------------------------------------+
| ``private var``  | 私有的可读写字段                               |
+------------------+------------------------------------------------+

访问修饰符
-----------

.. code-block:: scala

   class AccessDemo:
     private val secret = "隐藏"              // 仅本类可见
     protected val family = "子类可见"         // 本类 + 子类
     val public = "公开"                       // 默认：公开
     private[this] val instanceOnly = "仅此实例"   // 实例级别私有

特质（Trait）
==============

特质类似 Java 的接口，但可以包含实现字段和方法：

.. code-block:: scala

   trait HasName:
     def name: String           // 抽象方法
     def greet(): String =      // 具体实现方法
       s"Hello, I'm $name"

   trait HasAge:
     def age: Int
     def isAdult: Boolean = age >= 18

   // 混入多个特质
   class Person(val name: String, val age: Int)
     extends HasName, HasAge:
     override def greet(): String = s"Hi, I'm $name"

.. tip::

   Scala 用 ``extends`` + ``,`` 代替 Java 的 ``implements`` 关键字。
   特质中的 ``super`` 调用遵循线性化规则（类线性化）。

特质与抽象类的选择
-------------------

.. list-table::
   :header-rows: 1

   * - 特性
     - 特质 (trait)
     - 抽象类 (abstract class)
   * - 多继承
     - 支持（混入多个特质）
     - 不支持（单继承）
   * - 构造函数参数
     - 不支持
     - 支持
   * - 与 Java 互操作
     - 含实现的特质需 ``abstract class`` 桥接
     - 自然兼容

| **推荐优先使用特质**。

样例类（Case Class）
====================

样例类是 Scala 中的一等公民数据载体，编译器自动生成大量样板代码：

.. code-block:: scala

   case class User(id: Long, name: String, email: String)

自动生成的内容：

.. code-block:: scala

   // 1. 工厂方法（无需 new）
   val u1 = User(1, "Alice", "alice@example.com")

   // 2. 字段访问（val，不可变）
   u1.name   // "Alice"

   // 3. toString / hashCode / equals（值语义）
   User(1, "Alice", "a@b") == User(1, "Alice", "a@b")   // true

   // 4. copy 方法
   val u2 = u1.copy(email = "new@example.com")

   // 5. 模式匹配支持
   u1 match
     case User(id, _, _) => println(s"用户 ID: $id")

.. warning::

   样例类的相等性基于**值**而非引用。如果需要引用语义，使用普通 ``class``。

单例对象与伴生对象
===================

Scala 使用 ``object`` 关键字定义单例：

.. code-block:: scala

   object Config:
     val AppName = "MyApp"
     val Version = "1.0"
     def load(): Unit = println("加载配置...")

   // 直接调用
   println(Config.AppName)
   Config.load()

**伴生对象**：与类同名且在同一个文件中定义的 ``object``：

.. code-block:: scala

   class User(val name: String)
   // 伴生对象，持有静态工厂方法
   object User:
     def apply(name: String): User = new User(name)
     def anonymous: User = new User("guest")

   // 使用工厂方法
   val u1 = User("Alice")    // 等同于 User.apply("Alice")
   val u2 = User.anonymous

继承
=====

.. code-block:: scala

   // 基类
   class Animal:
     def sound: String = "..."

   // 子类，覆写方法需加 override
   class Dog extends Animal:
     override def sound: String = "Woof!"

   // 抽象类
   abstract class Shape:
     def area: Double          // 抽象方法，无实现

   class Circle(r: Double) extends Shape:
     override def area: Double = math.Pi * r * r

密封类型（Sealed）
===================

``sealed`` 关键字限定类型只能在同一个文件中被继承，
使模式匹配穷举检查成为可能：

.. code-block:: scala

   sealed trait TrafficLight
   case object Red    extends TrafficLight
   case object Yellow extends TrafficLight
   case object Green  extends TrafficLight

   def action(light: TrafficLight): String = light match
     case Red    => "停"
     case Yellow => "等待"
     case Green  => "行"
     // 编译器可验证匹配是否穷举

枚举（Scala 3）
===============

Scala 3 引入了原生枚举：

.. code-block:: scala

   enum Color:
     case Red, Green, Blue

   enum Suit(val symbol: String):
     case Hearts   extends Suit("♥")
     case Diamonds extends Suit("♦")
     case Clubs    extends Suit("♣")
     case Spades   extends Suit("♠")

   println(Suit.Hearts.symbol)   // ♥
