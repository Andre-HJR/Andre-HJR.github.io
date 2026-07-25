=========================
指针与内存管理
=========================

指针是 C 语言最强大也最危险的特性。理解指针是掌握 C 的关键。

指针基础
=========

什么是指针
----------

指针是一个**变量，存储的是内存地址**。

.. code-block:: c

   int  x = 42;          // 普通变量，存储值
   int *p = &x;          // 指针变量，存储 x 的地址

   printf("x 的值: %d\n", x);         // 42
   printf("x 的地址: %p\n", &x);      // 0x...
   printf("p 的值: %p\n", p);         // 与 &x 相同
   printf("p 指向的值: %d\n", *p);    // 42（解引用）

指针的声明与初始化
-------------------

.. code-block:: c

   int *p;               // 声明指针（未初始化，指向不确定位置）
   int *q = NULL;        // 初始化为空指针（安全）
   int *r = &x;          // 指向变量 x

   // 多个指针声明
   int *a, *b, *c;       // 三个指针（每个都要加 *）

.. warning::

   **未初始化的指针** (野指针) 指向不确定的内存地址。
   对野指针赋值或解引用可能导致难以调试的崩溃。

.. code-block:: c

   int *p;               // 野指针！
   *p = 100;             // 危险！可能写入非法内存

指针与类型
-----------

指针的类型决定了通过它读写内存的方式：

.. code-block:: c

   int    *p;     // 指向 int（4 字节）
   char   *q;     // 指向 char（1 字节）
   double *r;     // 指向 double（8 字节）

   int x = 0x12345678;
   int *p = &x;
   char *q = (char *)&x;

   printf("%x\n", *p);          // 0x12345678
   printf("%x\n", *q);          // 0x78（小端模式，取低字节）

.. note::

   ``void *`` 是通用指针类型，可以指向任何类型的数据，
   但**不能直接解引用**，必须先转换为具体类型。

指针运算
=========

.. code-block:: c

   int arr[5] = {10, 20, 30, 40, 50};
   int *p = arr;                 // 指向 arr[0]

   // 指针加/减整数：以指向的类型大小为步长
   printf("%d\n", *p);           // 10  (arr[0])
   printf("%d\n", *(p + 1));     // 20  (arr[1])
   printf("%d\n", *(p + 3));     // 40  (arr[3])

   p++;                          // 前进到 arr[1]
   printf("%d\n", *p);           // 20
   p--;                          // 回到 arr[0]

   // 指针相减：得到元素个数差值
   int *start = &arr[0];
   int *end   = &arr[4];
   printf("%d\n", end - start);  // 4（中间有 4 个元素）

.. important::

   指针运算的单位是**元素大小**，不是字节。
   ``int *p`` 执行 ``p + 1`` 实际地址增加 ``sizeof(int)`` 字节（4 字节）。

指针与 const
=============

.. code-block:: c

   int x = 10, y = 20;

   // 1. 指向常量的指针（不能改值，可以改指向）
   const int *p1 = &x;
   *p1 = 30;       // 错误！
   p1 = &y;        // 允许

   // 2. 常量指针（不能改指向，可以改值）
   int * const p2 = &x;
   *p2 = 30;       // 允许
   p2 = &y;        // 错误！

   // 3. 指向常量的常量指针（都不能改）
   const int * const p3 = &x;
   *p3 = 30;       // 错误！
   p3 = &y;        // 错误！

二级指针（指向指针的指针）
===========================

.. code-block:: c

   int x = 42;
   int *p = &x;        // 一级指针
   int **pp = &p;      // 二级指针

   printf("%d\n", x);        // 42
   printf("%d\n", *p);       // 42
   printf("%d\n", **pp);     // 42

   // 典型用途：在函数中修改指针本身
   void allocate(int **pp, int size) {
       *pp = (int *)malloc(size * sizeof(int));
   }

   int main(void) {
       int *arr = NULL;
       allocate(&arr, 10);   // arr 被修改为指向新分配的内存
       free(arr);
       return 0;
   }

动态内存分配
=============

C 语言提供 ``malloc``、``calloc``、``realloc``、``free`` 进行堆内存管理。

.. code-block:: c

   #include <stdlib.h>   // malloc, calloc, realloc, free
   #include <string.h>   // memset

   // 分配内存
   int *p = (int *)malloc(5 * sizeof(int));
   if (p == NULL) {
       // 内存分配失败
       printf("内存不足！\n");
       return 1;
   }

   // 使用内存
   for (int i = 0; i < 5; i++) {
       p[i] = i * 10;
   }

   // 释放内存
   free(p);
   p = NULL;   // 避免悬空指针

.. important::

   动态内存管理的**黄金法则**：

   #. 每次 ``malloc``/``calloc`` 必须对应一次 ``free``
   #. 释放后立即将指针置为 ``NULL``
   #. 访问前检查指针是否为 ``NULL``

malloc vs calloc
-----------------

.. code-block:: c

   // malloc：分配不初始化
   int *a = (int *)malloc(5 * sizeof(int));
   // a 中的值不确定（可能是旧数据）

   // calloc：分配并初始化为 0
   int *b = (int *)calloc(5, sizeof(int));
   // b 中所有元素为 0

realloc（调整大小）
--------------------

.. code-block:: c

   int *arr = (int *)malloc(5 * sizeof(int));

   // 扩展为 10 个元素
   int *new_arr = (int *)realloc(arr, 10 * sizeof(int));
   if (new_arr != NULL) {
       arr = new_arr;   // 注意：realloc 可能移动内存
   } else {
       // 扩展失败，原内存仍然有效
       printf("realloc 失败\n");
   }

常见内存错误
=============

.. code-block:: c

   // 1. 内存泄漏：忘记 free
   void leak(void) {
       int *p = (int *)malloc(100);
       // 没有 free(p) —— 内存泄漏！
   }

   // 2. 悬空指针：free 后还继续使用
   int *p = (int *)malloc(sizeof(int));
   free(p);
   *p = 42;    // 错误！已释放的内存

   // 3. 重复释放
   free(p);
   free(p);    // 错误！double free

   // 4. 越界访问
   int *arr = (int *)malloc(3 * sizeof(int));
   arr[3] = 42;     // 越界！堆块头信息可能被损坏

   // 5. 对 free 后的指针解引用
   int *ptr = (int *)malloc(sizeof(int));
   *ptr = 10;

   int *alias = ptr;  // 别名指向同一块内存
   free(ptr);
   *alias = 20;       // 错误！通过悬空指针写入

VC10 内存调试
===============

VC10 提供强大的内存泄漏检测工具：

.. code-block:: c

   #define _CRTDBG_MAP_ALLOC
   #include <stdlib.h>
   #include <crtdbg.h>

   int main(void) {
       // 启用内存泄漏检测
       _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |
                       _CRTDBG_LEAK_CHECK_DF);

       int *p = (int *)malloc(100);
       // 忘记 free —— 程序退出时会报告泄漏

       return 0;
   }

.. tip::

   VC10 内存检测技巧：

   #. 在程序入口调用 ``_CrtSetDbgFlag`` 启用泄漏检测
   #. 输出窗口会显示泄漏的内存块号
   #. 使用 ``_CrtSetBreakAlloc(块号)`` 在分配时中断
   #. 调用栈窗口可以查看是谁分配的内存
