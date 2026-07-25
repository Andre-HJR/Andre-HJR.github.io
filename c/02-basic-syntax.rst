============
基础语法
============

C 语言的语法简洁而强大，掌握这些核心概念是编写高质量代码的基础。

基本结构
=========

一个 C 程序由 **函数** 组成，程序的入口是 ``main`` 函数：

.. code-block:: c
   :caption: 程序基本结构

   #include <stdio.h>   // 预处理指令：引入头文件

   /* 全局变量 */
   int global_count = 0;

   /* 函数定义 */
   int add(int a, int b) {
       return a + b;
   }

   /* 主函数：程序入口 */
   int main(void) {
       int result = add(3, 4);
       printf("Result: %d\n", result);
       return 0;          // 返回 0 表示正常退出
   }

注释
=====

.. code-block:: c

   /* 这是多行注释
      可以跨越多行 */

   // 这是单行注释（C99 支持，VC10 中可用）

   int x = 10;   /* 行尾注释 */

数据类型
=========

基本数据类型
-------------

.. list-table::
   :header-rows: 1

   * - 类型
     - 大小
     - 取值范围
     - 格式化占位符
   * - ``char``
     - 1 字节
     - -128 ~ 127 或 0 ~ 255
     - ``%c`` / ``%d``
   * - ``int``
     - 4 字节
     - -2,147,483,648 ~ 2,147,483,647
     - ``%d``
   * - ``short``
     - 2 字节
     - -32,768 ~ 32,767
     - ``%hd``
   * - ``long``
     - 4 字节
     - -2,147,483,648 ~ 2,147,483,647
     - ``%ld``
   * - ``float``
     - 4 字节
     - 约 +/- 3.4e-38 ~ +/- 3.4e+38
     - ``%f``
   * - ``double``
     - 8 字节
     - 约 +/- 1.7e-308 ~ +/- 1.7e+308
     - ``%lf``

.. note::

   上述大小为 32 位平台下的典型值。VC10 在 64 位下 ``long`` 仍为 4 字节。

修饰符
-------

.. code-block:: c

   signed   int a;   // 有符号整数（默认）
   unsigned int b;   // 无符号整数（0 ~ 4,294,967,295）
   short    int c;   // 短整数
   long     int d;   // 长整数
   long long e;      // 64 位整数（C99，VC10 支持）

   // VC10 中的固定宽度类型（需 <stdint.h>）
   #include <stdint.h>
   int32_t  x;       // 32 位有符号
   uint64_t y;       // 64 位无符号
   size_t   z;       // 无符号大小类型（sizeof 返回值类型）

常量
-----

.. code-block:: c

   const int MAX = 100;          // 常变量
   #define PI 3.14159            // 宏常量（预处理阶段替换）
   #define ERROR_CODE -1

   enum Color { RED, GREEN, BLUE };  // 枚举常量（默认 0, 1, 2）

变量定义与初始化
=================

.. code-block:: c

   int a;                  // 声明变量（未初始化，值不确定）
   int b = 0;              // 定义并初始化
   int c = 1, d = 2;       // 同时定义多个变量

   /* VC10 C89 要求：变量声明必须在代码块开头 */
   void func(void) {
       int x = 10;         // OK：在开头声明
       printf("%d\n", x);

       // int y = 20;      // 错误：C89 不允许在语句之后声明
       // 要使用 -D_CRT_SECURE_NO_WARNINGS 或改用 C++ 模式

       {   /* 使用复合语句（代码块）解决 */
           int y = 20;
           printf("%d\n", y);
       }
   }

.. warning::

   VC10 在 C 模式下严格遵循 C89，**所有变量声明必须位于代码块的开头**，
   即在任何执行语句之前。C99 和 C++ 无此限制。

运算符
=======

算术运算符
----------

.. code-block:: c

   int a = 10, b = 3;
   a + b;     // 13  加法
   a - b;     // 7   减法
   a * b;     // 30  乘法
   a / b;     // 3   整数除法（截断）
   a % b;     // 1   取模（仅整数）
   a++;       // 自增（后置）
   ++a;       // 自增（前置）

.. important::

   整数除法会**截断**小数部分。若需浮点结果，至少一个操作数为浮点型：
   ``10 / (double)3`` → 3.333...

关系运算符与逻辑运算符
-----------------------

.. code-block:: c

   // 关系运算符：返回 1（真）或 0（假）
   a == b    // 等于
   a != b    // 不等于
   a > b     // 大于
   a < b     // 小于

   // 逻辑运算符
   !a        // 逻辑非
   a && b    // 逻辑与（短路求值）
   a || b    // 逻辑或（短路求值）

位运算符
--------

.. code-block:: c

   unsigned char x = 0b1100;  // VC10 不支持 0b 前缀
   unsigned char x = 0x0C;    // 十六进制写法

   a & b       // 按位与
   a | b       // 按位或
   a ^ b       // 按位异或
   ~a          // 按位取反
   a << 2      // 左移 2 位（等价于乘以 4）
   a >> 1      // 右移 1 位（等价于除以 2）

类型转换
=========

隐式转换
---------

.. code-block:: c

   int i = 10;
   double d = i;         // int → double，隐式提升

   double x = 3.14;
   int y = x;            // double → int，截断为 3（精度丢失）

显式转换（强制类型转换）
-------------------------

.. code-block:: c

   double pi = 3.14159;
   int approx = (int)pi;           // 3
   float f = (float)pi;            // double → float

   int a = 5, b = 2;
   double result = (double)a / b;  // 2.5（先转换再除法）

printf 格式化输出
==================

.. code-block:: c

   int    a = 42;
   double b = 3.14159;
   char   c = 'A';
   char   s[] = "Hello";

   printf("整数: %d\n", a);           // 42
   printf("浮点数: %.2f\n", b);       // 3.14
   printf("字符: %c\n", c);           // A
   printf("字符串: %s\n", s);         // Hello
   printf("十六进制: %x\n", a);       // 2a
   printf("指针: %p\n", &a);          // 内存地址

.. tip::

   VC10 的 ``printf`` 需注意：
   - ``%I64d`` 用于 ``long long``（非 ``%lld``）
   - ``%zu`` 用于 ``size_t``（C99 特性，可能出现警告）
   - 可使用 ``%Iu`` 替代 ``%zu``
