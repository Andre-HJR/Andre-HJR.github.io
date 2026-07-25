==============
控制结构
==============

C 语言提供了三类基本控制结构：顺序、选择（分支）和循环。

选择结构
=========

if-else 语句
-------------

.. code-block:: c

   int score = 85;

   if (score >= 90) {
       printf("优秀\n");
   } else if (score >= 80) {
       printf("良好\n");
   } else if (score >= 60) {
       printf("及格\n");
   } else {
       printf("不及格\n");
   }

.. tip::

   即使只有一条语句，也**始终使用大括号** ``{}``，避免维护时出错。

条件表达式（三元运算符）
-------------------------

.. code-block:: c

   int x = 10, y = 20;
   int max = (x > y) ? x : y;   // 如果 x > y 取 x，否则取 y

   // 嵌套使用（可读性较差，谨慎使用）
   int sign = (x > 0) ? 1 : (x < 0) ? -1 : 0;

switch-case 语句
-----------------

.. code-block:: c

   int op = 2;

   switch (op) {
       case 1:
           printf("新增\n");
           break;
       case 2:
           printf("修改\n");
           break;
       case 3:
           printf("删除\n");
           break;
       default:
           printf("未知操作\n");
           break;
   }

.. warning::

   - 每个 ``case`` 后必须有 ``break``，否则会**穿透**（fall-through）到下一个 case
   - 有意利用穿透时请加注释：``/* fall through */``
   - ``switch`` 表达式必须是整数类型（``int``、``char``、``enum``）
   - VC10 不支持 C99 的 ``case`` 范围扩展（``case 1 ... 5:``）

循环结构
=========

while 循环
-----------

.. code-block:: c

   int i = 1;
   while (i <= 5) {
       printf("%d ", i);
       i++;
   }
   // 输出: 1 2 3 4 5

do-while 循环
--------------

.. code-block:: c

   int i = 1;
   do {
       printf("%d ", i);
       i++;
   } while (i <= 5);
   // 输出: 1 2 3 4 5

.. note::

   ``do-while`` **至少执行一次** 循环体，适用于需要先执行再判断的场景。

for 循环
---------

.. code-block:: c

   // 标准 for 循环
   for (int i = 0; i < 5; i++) {
       printf("%d ", i);
   }
   // 输出: 0 1 2 3 4

   // VC10 C 模式注意事项：上面的写法是 C++ 风格
   // C89 风格必须将变量声明在循环外：
   int i;
   for (i = 0; i < 5; i++) {
       printf("%d ", i);
   }

.. caution::

   VC10 在 C 模式下编译 ``.c`` 文件时，``for (int i = 0;;)`` 这种声明会报错，
   因为 C89 不允许在 ``for`` 初始化部分声明变量。有两种解决方法：

   #. 将变量声明在函数开头
   #. 将文件扩展名改为 ``.cpp`` (使用 C++ 编译器)

   VC10 的 C 编译器不支持 C99 的 ``for`` 循环内声明。

循环控制
=========

.. code-block:: c

   // break：立即退出循环
   for (int i = 0; i < 10; i++) {
       if (i == 5) break;      // i = 5 时退出循环
       printf("%d ", i);
   }
   // 输出: 0 1 2 3 4

   // continue：跳过当前迭代，进入下一次
   for (int i = 0; i < 10; i++) {
       if (i % 2 == 0) continue;   // 跳过偶数
       printf("%d ", i);
   }
   // 输出: 1 3 5 7 9

   // goto：跳转到指定标签（谨慎使用）
   int error = 1;
   if (error) {
       goto cleanup;
   }
   // ...
   cleanup:
       printf("清理资源\n");

.. important::

   ``goto`` 仅在特殊情况（如多层嵌套循环直接跳出、统一错误处理）中使用。
   过度使用会破坏程序的结构化特性。

嵌套循环
=========

.. code-block:: c

   // 打印乘法口诀表
   for (int i = 1; i <= 9; i++) {
       for (int j = 1; j <= i; j++) {
           printf("%d×%d=%-2d ", j, i, i * j);
       }
       printf("\n");
   }

   // 输出：
   // 1×1=1
   // 1×2=2  2×2=4
   // 1×3=3  2×3=6  3×3=9
   // ...

综合示例：猜数字游戏
======================

.. code-block:: c

   #include <stdio.h>
   #include <stdlib.h>
   #include <time.h>

   int main(void) {
       int target, guess, attempts = 0;

       srand((unsigned)time(NULL));
       target = rand() % 100 + 1;   // 1 ~ 100

       printf("猜数字游戏 (1~100)\n");

       do {
           printf("请输入你的猜测: ");
           scanf("%d", &guess);
           attempts++;

           if (guess > target) {
               printf("太大了！\n");
           } else if (guess < target) {
               printf("太小了！\n");
           } else {
               printf("恭喜！猜中了！共用了 %d 次\n", attempts);
               break;
           }
       } while (1);   // 无限循环，靠 break 退出

       return 0;
   }

.. tip::

   VC10 中 ``scanf`` 可能会报告安全问题，可使用 ``scanf_s`` 替代：

   .. code-block:: c

      scanf_s("%d", &guess);
