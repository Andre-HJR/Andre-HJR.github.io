===================
Dart 语言基础
===================

Dart 是 Flutter 的开发语言，由 Google 设计，兼具 JIT（开发期快速迭代）和
AOT（发布期高性能编译）能力。

变量与类型
===========

类型声明
---------

.. code-block:: dart

   // 显式类型
   String name = 'Dart';
   int age = 10;

   // 类型推断（var）
   var message = 'Hello';        // 推断为 String
   var count = 42;               // 推断为 int

   // 动态类型（dynamic）
   dynamic value = 'Hello';
   value = 123;                  // 可重新赋值为其他类型

   // 常量
   final String appName = 'MyApp';     // 运行时赋值一次
   const double pi = 3.14159;           // 编译时常量

.. tip::

   - ``final``：值在运行期确定，赋值后不可变
   - ``const``：值在编译期已知，且不可变
   - ``const`` 对象称为编译时常量，Dart 会将其规范化

内置类型
---------

.. code-block:: dart

   // 数字
   int i = 42;
   double d = 3.14;
   num n = 42;        // int 和 double 的父类型

   // 字符串
   String s1 = '单引号';
   String s2 = "双引号";
   String s3 = '''多行
   字符串''';
   String msg = 'Hello, $s1!';           // 插值
   String calc = '2 + 3 = ${2 + 3}';     // 表达式插值

   // 布尔
   bool isDone = true;
   bool isActive = false;

   // 列表（数组）
   List<int> numbers = [1, 2, 3];
   var fruits = ['apple', 'banana'];
   fruits.add('orange');
   print(fruits[0]);          // apple

   // 集合
   Set<int> unique = {1, 2, 3, 3};    // {1, 2, 3}

   // 映射
   Map<String, int> scores = {
     'Alice': 95,
     'Bob': 87,
   };
   scores['Charlie'] = 92;

空安全（Null Safety）
=====================

Dart 全面支持空安全，类型默认不可为 ``null``：

.. code-block:: dart

   String name = 'Alice';
   // name = null;       // 编译错误！

   String? nullable = null;    // 可空类型，使用 ?
   nullable = 'Bob';

   // 安全访问操作符
   print(nullable?.length);    // null 安全访问

   // 空值合并操作符
   String display = nullable ?? '默认值';

   // 断言非空（谨慎使用）
   String definitely = nullable!;

.. warning::

   使用 ``!`` 断言非空前，确保变量确实不为 ``null``，否则运行时会抛出异常。

函数
=========

基本用法
---------

.. code-block:: dart

   // 标准函数
   int add(int a, int b) {
     return a + b;
   }

   // 箭头函数（单表达式）
   int square(int x) => x * x;

   // 可选命名参数
   void greet({required String name, int age = 18}) {
     print('Hello, $name ($age)');
   }
   greet(name: 'Alice', age: 30);
   greet(name: 'Bob');             // age 使用默认值

   // 可选位置参数
   String join(String a, [String? b, String? c]) {
     return '$a${b ?? ''}${c ?? ''}';
   }
   join('a');          // 'a'
   join('a', 'b');     // 'ab'

函数是一等公民
--------------

.. code-block:: dart

   // 函数赋值给变量
   var fn = (int x) => x * 2;
   print(fn(5));       // 10

   // 高阶函数
   void execute(int x, int Function(int) op) {
     print(op(x));
   }
   execute(5, (x) => x * x);   // 25

   // 匿名函数
   [1, 2, 3].map((e) => e * 2).toList();   // [2, 4, 6]

类与对象
=========

.. code-block:: dart

   class Person {
     final String name;    // 不可变属性
     int age;

     // 构造函数
     Person(this.name, this.age);

     // 命名构造函数
     Person.guest() : this('Guest', 0);

     // 命名构造函数 + 初始化列表
     Person.fromJson(Map<String, dynamic> json)
         : name = json['name'] as String,
           age = json['age'] as int;

     // 方法
     void introduce() {
       print('Hi, I\'m $name, $age years old.');
     }

     // getter
     bool get isAdult => age >= 18;

     // setter
     set birthday(int year) {
       age = DateTime.now().year - year;
     }
   }

   // 使用
   final alice = Person('Alice', 30);
   alice.introduce();
   print(alice.isAdult);     // true

继承与混入
-----------

.. code-block:: dart

   class Animal {
     void sound() => print('...');
   }

   // 继承
   class Dog extends Animal {
     @override
     void sound() => print('Woof!');
   }

   // 混入（Mixin）
   mixin Flyable {
     void fly() => print('Flying!');
   }

   mixin Swimmable {
     void swim() => print('Swimming!');
   }

   // 使用混入（可以多个）
   class Duck extends Animal with Flyable, Swimmable {
     @override
     void sound() => print('Quack!');
   }

.. tip::

   混入（Mixin）是 Dart 代码复用的重要方式，比多重继承更灵活安全。

抽象类与接口
------------

.. code-block:: dart

   // 抽象类
   abstract class Shape {
     double get area;      // 抽象 getter
     void draw();          // 抽象方法
   }

   // 实现（所有类都隐式定义了接口）
   class Circle implements Shape {
     final double radius;
     Circle(this.radius);

     @override
     double get area => 3.14 * radius * radius;

     @override
     void draw() => print('Drawing Circle');
   }

异步编程
=========

Future
------

.. code-block:: dart

   // 异步函数返回 Future
   Future<String> fetchData() async {
     await Future.delayed(Duration(seconds: 1));
     return '数据加载完成';
   }

   // 使用 async/await
   void load() async {
     print('加载中...');
     String data = await fetchData();
     print(data);
   }

   // Future 链式调用
   fetchData()
     .then((data) => print(data))
     .catchError((error) => print('出错: $error'))
     .whenComplete(() => print('完成'));

Stream
------

.. code-block:: dart

   // 创建 Stream
   Stream<int> countStream(int max) async* {
     for (int i = 1; i <= max; i++) {
       await Future.delayed(Duration(seconds: 1));
       yield i;           // 产出值
     }
   }

   // 消费 Stream
   void listen() async {
     await for (final value in countStream(3)) {
       print(value);     // 每秒输出 1, 2, 3
     }
   }

.. note::

   ``async*`` 和 ``yield`` 用于生成 Stream，``await for`` 用于消费 Stream。
