============
C# 语言基础
============

类型系统
=========

值类型 vs 引用类型
------------------

.. list-table::
   :header-rows: 1

   * - 特性
     - 值类型 (struct)
     - 引用类型 (class)
   * - 存储位置
     - 栈（Stack）
     - 堆（Heap）
   * - 赋值行为
     - 复制整个值
     - 复制引用（地址）
   * - 默认值
     - 0 / false / null?
     - null
   * - 继承
     - 不能继承
     - 可以继承
   * - 示例
     - ``int``, ``double``, ``bool``, ``DateTime``
     - ``string``, ``object``, ``Array``, ``自定义类``

.. code-block:: csharp

   // 值类型
   int a = 10;
   int b = a;        // 复制值
   b = 20;
   Console.WriteLine(a);  // 10（a 不变）

   // 引用类型
   var list1 = new List<int> { 1, 2, 3 };
   var list2 = list1;     // 复制引用（指向同一对象）
   list2.Add(4);
   Console.WriteLine(list1.Count);  // 4（list1 也被修改！）

内置类型
---------

.. code-block:: csharp

   // 整数类型
   sbyte   sb = 100;    // 8 位有符号
   short   s  = 1000;   // 16 位
   int     i  = 100000; // 32 位（默认）
   long    l  = 100L;   // 64 位
   nint    ni = 42;     // 平台相关大小（native int）

   // 无符号
   byte    ub = 255;    // 8 位
   ushort  us = 5000;
   uint    ui = 4000000000;
   ulong   ul = 100UL;

   // 浮点
   float   f = 3.14f;   // 32 位, ~7 位精度
   double  d = 3.14;    // 64 位, ~15 位精度（默认）
   decimal m = 3.14m;   // 128 位, 28 位精度（财务计算）

   // 其他
   bool    b = true;
   char    c = 'A';      // 16 位 Unicode
   string  s = "Hello";  // 引用类型
   object  o = 42;       // 所有类型的基类

var 关键字（类型推断）
-----------------------

.. code-block:: csharp

   var name = "Alice";        // 推断为 string
   var age = 30;              // 推断为 int
   var list = new List<int>(); // 推断为 List<int>
   var dict = new Dictionary<string, int>();

   // var 不是动态类型——编译时类型已确定！
   // name = 42;  ← 编译错误！

可空类型
---------

.. code-block:: csharp

   // Nullable value types
   int? nullableInt = null;
   double? nullableDouble = null;
   bool? isComplete = null;   // 3-state: true / false / null

   // Operations on nullable types
   int? a = null;
   int  b = a ?? 0;           // null coalescing: use 0 when a is null
   int  c = a.GetValueOrDefault(10);

字符串
=======

.. code-block:: csharp

   string str1 = "Hello";
   string str2 = @"C:\Users\name";        // verbatim string
   string str3 = $"Hello, {name}!";       // string interpolation

   // Common operations
   str1.Length;                   // 5
   str1.ToUpper();                // "HELLO"
   str1.Substring(0, 2);          // "He"
   str1.Replace("l", "L");        // "HeLLo"
   string.Join(", ", [1, 2, 3]);  // "1, 2, 3"
   str1.Contains("el");           // true

控制流
========

.. code-block:: csharp

   // if 语句
   if (score >= 90)
       grade = "A";
   else if (score >= 80)
       grade = "B";
   else
       grade = "C";

   // switch 表达式（C# 7+）
   var grade = score switch
   {
       >= 90 => "A",
       >= 80 => "B",
       >= 60 => "C",
       _     => "D"       // default
   };

   // 模式匹配（C# 7+）
   object obj = 42;
   if (obj is int number)
       Console.WriteLine($"整数: {number}");

   switch (obj)
   {
       case int i when i > 0:
           Console.WriteLine($"正数: {i}");
           break;
       case string s:
           Console.WriteLine($"字符串: {s}");
           break;
       case null:
           Console.WriteLine("null");
           break;
   }

   // 循环
   for (int i = 0; i < 5; i++) { }
   foreach (var item in collection) { }
   while (condition) { }
   do { } while (condition);

集合
=======

.. code-block:: csharp

   // 数组（固定大小）
   int[] arr = [1, 2, 3, 4, 5];       // 集合表达式（C# 12+）
   int[] arr2 = new int[] { 1, 2, 3 };
   int[,] matrix = new int[3, 4];
   arr[0] = 10;

   // List（动态大小）
   var list = new List<int> { 1, 2, 3 };
   list.Add(4);
   list.Remove(2);
   list[0] = 10;

   // Dictionary
   var scores = new Dictionary<string, int>
   {
       ["Alice"] = 95,
       ["Bob"] = 87
   };
   scores["Charlie"] = 92;

   // HashSet
   var unique = new HashSet<int> { 1, 2, 3, 3 };  // {1, 2, 3}

   // LINQ 集成
   var result = list.Where(x => x > 5)
                    .OrderBy(x => x)
                    .Select(x => x * 2)
                    .ToList();

异常处理
=========

.. code-block:: csharp

   try
   {
       var data = File.ReadAllText("data.txt");
       var num = int.Parse(data);
   }
   catch (FileNotFoundException ex)
   {
       Console.WriteLine($"文件不存在: {ex.FileName}");
   }
   catch (FormatException ex)
   {
       Console.WriteLine($"格式错误: {ex.Message}");
   }
   catch (Exception ex)
   {
       Console.WriteLine($"未知错误: {ex.Message}");
       throw;  // 保留调用栈重新抛出
   }
   finally
   {
       // 一定会执行（清理资源）
       reader?.Dispose();
   }

   // using 语句（自动释放）
   using var file = new StreamReader("data.txt");
   var content = file.ReadToEnd();
   // 离开作用域时自动 Dispose
