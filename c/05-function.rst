============
函数
============

函数是 C 语言中组织代码的基本单元，也是结构化编程的核心。

函数定义
=========

.. code-block:: c

   返回值类型 函数名(参数列表) {
       函数体
       return 返回值;
   }

.. code-block:: c
   :caption: 完整示例

   #include <stdio.h>

   // 函数声明（原型）
   int add(int a, int b);
   void print_message(void);

   // 函数定义
   int add(int a, int b) {
       return a + b;
   }

   void print_message(void) {
       printf("Hello from function!\n");
       // 无返回值，不需要 return
   }

   int main(void) {
       int sum = add(3, 4);          // 调用
       printf("Sum = %d\n", sum);     // 7

       print_message();
       return 0;
   }

.. important::

   在 C 语言中，**函数必须在使用前声明或定义**。
   通常将函数声明放在文件开头或头文件中。

参数传递
=========

值传递（传值）
--------------

C 语言中**默认都是值传递**：

.. code-block:: c

   void swap_wrong(int a, int b) {
       int temp = a;
       a = b;
       b = temp;         // 只交换了形参，不影响实参
   }

   int main(void) {
       int x = 3, y = 5;
       swap_wrong(x, y);
       printf("%d %d\n", x, y);   // 3 5（没有交换！）
       return 0;
   }

地址传递（传指针）
--------------------

利用指针实现真正的交换：

.. code-block:: c

   void swap(int *a, int *b) {
       int temp = *a;    // 通过指针访问实参
       *a = *b;
       *b = temp;
   }

   int main(void) {
       int x = 3, y = 5;
       swap(&x, &y);              // 传递地址
       printf("%d %d\n", x, y);   // 5 3（成功交换）
       return 0;
   }

.. tip::

   传指针是 C 语言中模拟"引用传递"的方式，用于：
   - 修改调用者的变量
   - 避免复制大结构体（提高性能）
   - 动态内存分配

数组作为参数
--------------

.. code-block:: c

   // 数组作为参数时退化为指针
   // 以下三种声明等价：
   int sum(int arr[], int n);
   int sum(int *arr, int n);
   int sum(int arr[10], int n);   // 编译器忽略 10

   // 必须同时传递数组长度！
   int sum(int arr[], int n) {
       int total = 0;
       for (int i = 0; i < n; i++) {
           total += arr[i];
       }
       return total;
   }

   int main(void) {
       int nums[] = {1, 2, 3, 4, 5};
       int result = sum(nums, 5);         // 15
       return 0;
   }

.. caution::

   数组参数退化为指针后，函数内 ``sizeof(arr)`` 得到的是**指针大小**，不是数组大小。

返回值
=======

.. code-block:: c

   // 返回值
   int square(int x) {
       return x * x;
   }

   // 返回指针
   int *find_max(int *arr, int n) {
       int *max = &arr[0];
       for (int i = 1; i < n; i++) {
           if (arr[i] > *max) max = &arr[i];
       }
       return max;           // 返回指向最大值的指针
   }

.. warning::

   **永远不要返回局部变量的地址！**

   .. code-block:: c

      int *bad_func(void) {
          int local = 42;
          return &local;     // 错误！函数返回后 local 已销毁
      }

   可用 ``static`` 或动态内存分配解决：

   .. code-block:: c

      int *good_func(void) {
          static int value = 42;   // 静态变量，生命周期贯穿整个程序
          return &value;
      }

函数原型（声明）
=================

.. code-block:: c

   // 函数原型告诉编译器函数的存在
   int multiply(int x, int y);     // 完整原型
   int multiply(int, int);         // 可省略参数名

   // 无参数函数
   void version1(void);            // 明确表示无参数
   void version2();                // 旧式风格，参数不确定（不推荐）

.. note::

   VC10 中 ``int func()`` 和 ``int func(void)`` 略有不同：
   - ``func(void)``：明确无参数，调用时传参导致编译错误
   - ``func()``：参数未指定，调用时传参仅产生警告

作用域与生命周期
=================

.. list-table::
   :header-rows: 1

   * - 存储类别
     - 关键字
     - 作用域
     - 生命周期
     - 初始值
   * - 自动
     - (无)
     - 代码块内
     - 函数调用期间
     - 不确定
   * - 寄存器
     - ``register``
     - 代码块内
     - 函数调用期间
     - 不确定
   * - 静态局部
     - ``static``
     - 代码块内
     - 整个程序运行期
     - 0
   * - 全局
     - (无)
     - 整个文件
     - 整个程序运行期
     - 0
   * - 静态全局
     - ``static``
     - 本文件内
     - 整个程序运行期
     - 0
   * - 外部
     - ``extern``
     - 整个程序
     - 整个程序运行期
     - 0

.. code-block:: c
   :caption: 作用域示例

   #include <stdio.h>

   int global = 100;              // 全局变量（所有文件可见）

   static int file_static = 200;  // 静态全局变量（仅本文件可见）

   void counter(void) {
       static int count = 0;      // 静态局部变量
       count++;
       printf("调用次数: %d\n", count);
   }

   int main(void) {
       counter();    // 1
       counter();    // 2
       counter();    // 3

       int local = 50;            // 局部变量
       printf("global = %d\n", global);
       return 0;
   }

递归函数
=========

.. code-block:: c

   // 阶乘：n! = n × (n-1)!
   long long factorial(int n) {
       if (n <= 1) return 1;               // 基线条件
       return n * factorial(n - 1);         // 递归调用
   }

   // 斐波那契数列
   long long fibonacci(int n) {
       if (n <= 1) return n;               // 基线条件
       return fibonacci(n - 1) + fibonacci(n - 2);
   }

   int main(void) {
       printf("5! = %lld\n", factorial(5));       // 120

       for (int i = 0; i < 10; i++) {
           printf("%lld ", fibonacci(i));
       }
       printf("\n");   // 0 1 1 2 3 5 8 13 21 34

       return 0;
   }

.. caution::

   - 递归必须有**基线条件**（终止条件），否则会无限递归导致栈溢出
   - 每次递归调用都消耗栈空间，过深的递归可能导致堆栈溢出
   - VC10 默认栈大小约 1MB，可通过链接器选项 ``/STACK`` 调整

inline 函数（C99）
==================

.. code-block:: c

   // VC10 支持 __inline 关键字（C99 的 inline）
   __inline int max(int a, int b) {
       return (a > b) ? a : b;
   }

   // 使用 inline 减少函数调用开销
   // 但编译器是否真正内联由优化级别决定

函数指针
=========

.. code-block:: c

   #include <stdio.h>

   int add(int a, int b) { return a + b; }
   int sub(int a, int b) { return a - b; }
   int mul(int a, int b) { return a * b; }

   int main(void) {
       // 函数指针数组
       int (*ops[])(int, int) = {add, sub, mul};

       int a = 10, b = 5;
       printf("add: %d\n", ops[0](a, b));   // 15
       printf("sub: %d\n", ops[1](a, b));   // 5
       printf("mul: %d\n", ops[2](a, b));   // 50

       return 0;
   }

.. tip::

   函数指针常用于：
   - 回调函数（如 ``qsort`` 的比较函数）
   - 策略模式（替换不同的算法实现）
   - 状态机转换表
