============
集合框架
============

Scala 的集合库提供了丰富且功能强大的不可变和可变集合，并内置了大量高阶操作。

集合层次结构
==============

.. code-block::

                 Iterable
              /           \
           Seq           Map            Set
         /    \          /   \          /   \
      Indexed Linear  Sorted HashMap  Sorted HashSet
        |       |        |      |       |      |
     Vector   List    TreeMap ...   TreeSet ...

.. list-table::
   :header-rows: 1

   * - 类别
     - 不可变 (immutable)
     - 可变 (mutable)
   * - 序列（有序）
     - ``List``, ``Vector``, ``Range``, ``LazyList``
     - ``ArrayBuffer``, ``ListBuffer``
   * - 集合（无序唯一）
     - ``Set``, ``HashSet``, ``SortedSet``
     - ``mutable.Set``, ``mutable.HashSet``
   * - 映射（键值对）
     - ``Map``, ``HashMap``, ``SortedMap``
     - ``mutable.Map``, ``mutable.HashMap``

.. important::

   **默认导入的是不可变集合** (``scala.collection.immutable``)。
   使用可变集合需显式引入 ``scala.collection.mutable``。

常用序列
=========

List（单向链表）
-----------------

.. code-block:: scala

   val list = List(1, 2, 3, 4, 5)
   val head = list.head     // 1（首元素）
   val tail = list.tail     // List(2, 3, 4, 5)（除首元素外的剩余）
   val prepend = 0 :: list   // List(0, 1, 2, 3, 4, 5)
   val append  = list :+ 6   // List(1, 2, 3, 4, 5, 6)

Vector（随机访问序列）
-----------------------

.. code-block:: scala

   val vec = Vector(1, 2, 3, 4, 5)
   vec(0)              // 1（索引访问，O(log n)）
   vec.updated(0, 99)  // Vector(99, 2, 3, 4, 5)
   vec :+ 6            // Vector(1, 2, 3, 4, 5, 6)

.. tip::

   ``Vector`` 在大多数场景下是 ``List`` 的更好替代——随机访问更快，追加操作效率也高。

Range（数值范围）
------------------

.. code-block:: scala

   1 to 5          // Range(1, 2, 3, 4, 5)
   1 until 5       // Range(1, 2, 3, 4)
   1 to 10 by 2    // Range(1, 3, 5, 7, 9)
   (5 to 1 by -1).toList  // List(5, 4, 3, 2, 1)

集合核心操作
=============

转换操作
---------

.. code-block:: scala

   val nums = List(1, 2, 3, 4, 5)

   // map: 对每个元素应用函数
   nums.map(_ * 2)           // List(2, 4, 6, 8, 10)

   // flatMap: map + 展平
   nums.flatMap(x => List(x, x * 10))
   // List(1, 10, 2, 20, 3, 30, 4, 40, 5, 50)

   // flatten: 展平嵌套集合
   List(List(1, 2), List(3, 4)).flatten  // List(1, 2, 3, 4)

筛选操作
---------

.. code-block:: scala

   val nums = List(1, 2, 3, 4, 5, 6)

   // filter: 保留满足条件的元素
   nums.filter(_ % 2 == 0)         // List(2, 4, 6)

   // filterNot: 排除满足条件的元素
   nums.filterNot(_ % 2 == 0)      // List(1, 3, 5)

   // take/drop: 取前 N 个 / 丢弃前 N 个
   nums.take(3)                    // List(1, 2, 3)
   nums.drop(3)                    // List(4, 5, 6)

   // distinct: 去重
   List(1, 1, 2, 2, 3).distinct    // List(1, 2, 3)

聚合操作
---------

.. code-block:: scala

   val nums = List(1, 2, 3, 4, 5)

   // reduce: 用二元操作归约
   nums.reduce(_ + _)        // 15

   // fold: 带初始值的归约
   nums.fold(0)(_ + _)       // 15
   nums.foldLeft(0)(_ + _)   // 15（从左到右）
   nums.foldRight(0)(_ + _)  // 15（从右到左）

   // 计数与判断
   nums.count(_ > 3)         // 2
   nums.exists(_ > 10)       // false
   nums.forall(_ > 0)        // true

   // 最值
   nums.max                  // 5
   nums.min                  // 1
   nums.sum                  // 15

排序操作
---------

.. code-block:: scala

   val nums = List(3, 1, 4, 1, 5, 9)

   nums.sorted               // List(1, 1, 3, 4, 5, 9)
   nums.sortBy(-_)           // List(9, 5, 4, 3, 1, 1)（按降序）
   nums.sortWith(_ > _)      // List(9, 5, 4, 3, 1, 1)（自定义比较）

分组操作
---------

.. code-block:: scala

   val people = List("Alice" -> 30, "Bob" -> 25, "Charlie" -> 30)

   // groupBy: 按函数分组
   people.groupBy(_._2)
   // Map(30 -> List((Alice,30), (Charlie,30)), 25 -> List((Bob,25)))

   // partition: 按条件分两组
   List(1, 2, 3, 4, 5).partition(_ % 2 == 0)
   // (List(2, 4), List(1, 3, 5))

Map 操作
=========

.. code-block:: scala

   val scores = Map("Alice" -> 95, "Bob" -> 87)

   scores("Alice")               // 95（不存在时抛出异常）
   scores.get("Alice")           // Some(95)
   scores.get("Charlie")         // None
   scores.getOrElse("Charlie", 0)  // 0

   // 增删改（不可变 Map 返回新 Map）
   scores + ("Charlie" -> 92)       // 添加
   scores - "Alice"                 // 删除
   scores.updated("Bob", 90)        // 更新

   // 遍历
   for (name, score) <- scores do
     println(s"$name: $score")

Set 操作
=========

.. code-block:: scala

   val set = Set(1, 2, 3, 3, 2)  // Set(1, 2, 3)
   set + 4                        // Set(1, 2, 3, 4)
   set - 1                        // Set(2, 3)
   set.contains(2)                // true

   // 集合运算
   val a = Set(1, 2, 3)
   val b = Set(3, 4, 5)
   a.union(b)          // Set(1, 2, 3, 4, 5)
   a.intersect(b)      // Set(3)
   a.diff(b)           // Set(1, 2)

视图（View）
=============

视图是惰性集合——操作不会立即执行，只在需要时求值：

.. code-block:: scala

   val large = (1 to 1_000_000).view
     .filter(_ % 2 == 0)
     .map(_ * 3)
     .take(10)
     .toList
   // 只处理了前若干个元素，而非整个百万级集合

.. tip::

   视图适合链式操作中的**多个转换步骤**且有提前截断（``take``）的场景，
   能显著减少中间集合的创建开销。
