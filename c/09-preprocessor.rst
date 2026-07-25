==================
预处理与宏
==================

预处理器是 C 编译的**第一步**，负责处理所有以 ``#`` 开头的指令。

预处理工作流程
===============

.. code-block::

   源文件 (.c)    预处理    纯净 C 代码    编译     目标代码 (.obj)
       │         ──────→              ──────→
       │        #include             编译器
       │        #define
       │        #ifdef / #if
       │        #pragma

.. code-block:: bash

   # 查看预处理结果（VC10 开发人员命令提示符）
   cl /P main.c       # 生成 main.i（预处理后的文件）
   cl /E main.c       # 直接输出到控制台

宏定义（#define）
==================

简单宏
-------

.. code-block:: c

   #define PI          3.14159
   #define MAX_SIZE    100
   #define APP_NAME    "MyApp"
   #define DEBUG       1

   int arr[MAX_SIZE];             // 展开为 int arr[100];
   printf("App: %s\n", APP_NAME); // 展开为 "MyApp"

带参数宏（类函数宏）
--------------------

.. code-block:: c

   #define SQUARE(x)   ((x) * (x))
   #define MAX(a, b)   ((a) > (b) ? (a) : (b))
   #define MIN(a, b)   ((a) < (b) ? (a) : (b))

.. warning::

   带参宏中**参数和整体都要加括号**，否则优先级会出问题：

   .. code-block:: c

      #define SQUARE_BAD(x)  x * x

      int y = SQUARE_BAD(2 + 3);  // 2 + 3 * 2 + 3 = 11（错误！）
      int z = SQUARE(2 + 3);      // ((2 + 3) * (2 + 3)) = 25（正确）

多行宏
-------

.. code-block:: c

   #define LOG(msg)                     \
       do {                             \
           printf("[LOG] %s:%d: ",      \
                  __FILE__, __LINE__);   \
           printf("%s\n", msg);         \
       } while (0)

   // 使用
   LOG("开始处理");
   int x = 42;
   LOG("处理完成");

.. tip::

   多行宏使用 ``do { ... } while (0)`` 包裹，确保宏展开后语义正确
   （例如在 ``if-else`` 语句中使用时不会出问题）。

条件编译
=========

#ifdef / #ifndef / #endif
--------------------------

.. code-block:: c

   #define DEBUG 1

   #ifdef DEBUG
       printf("调试信息: x = %d\n", x);
   #endif

   #ifndef NDEBUG
       // 非发布版本的断言
       #define ASSERT(cond) \
           if (!(cond)) { \
               printf("断言失败: %s, file %s, line %d\n", \
                      #cond, __FILE__, __LINE__); \
           }
   #else
       #define ASSERT(cond) ((void)0)
   #endif

#if / #elif / #else
--------------------

.. code-block:: c

   #define PLATFORM 2

   #if PLATFORM == 1
       #include "windows.h"
       typedef int HANDLE;
   #elif PLATFORM == 2
       #include "linux.h"
       typedef int FD;
   #else
       #error "不支持的平台！"
   #endif

defined 运算符
---------------

.. code-block:: c

   #if defined(_WIN32) && !defined(_WIN64)
       // 32 位 Windows
       #define PLATFORM "Win32"
   #elif defined(_WIN64)
       // 64 位 Windows
       #define PLATFORM "Win64"
   #endif

文件包含（#include）
=====================

.. code-block:: c

   // 标准库头文件（在系统路径中查找）
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>

   // 用户自定义头文件（在当前目录/项目路径中查找）
   #include "myheader.h"
   #include "utils/helpers.h"

防止头文件重复包含
-------------------

.. code-block:: c
   :caption: myheader.h

   #ifndef MYHEADER_H
   #define MYHEADER_H

   /* 头文件内容 */
   int add(int a, int b);
   #define VERSION "1.0"

   #endif /* MYHEADER_H */

.. note::

   另一种方式是使用 ``#pragma once`` (VC10 支持)：

   .. code-block:: c

      #pragma once
      // 效果同 include guard，但更简洁

预定义宏
=========

VC10 常见预定义宏：

.. code-block:: c

   #include <stdio.h>

   int main(void) {
       printf("文件名:      %s\n", __FILE__);
       printf("行号:        %d\n", __LINE__);
       printf("日期:        %s\n", __DATE__);
       printf("时间:        %s\n", __TIME__);
       printf("ANSI 标准:   %d\n", __STDC__);
       printf("VC 版本:     %d\n", _MSC_VER);

   #ifdef _DEBUG
       printf("调试版本\n");
   #else
       printf("发布版本\n");
   #endif
   #ifdef _WIN64
       printf("64 位编译\n");
   #else
       printf("32 位编译\n");
   #endif

       return 0;
   }

.. list-table::
   :header-rows: 1

   * - 宏
     - 说明
   * - ``__FILE__``
     - 当前源文件路径（字符串）
   * - ``__LINE__``
     - 当前行号（整数）
   * - ``__DATE__``
     - 编译日期（如 "Jul 25 2026"）
   * - ``__TIME__``
     - 编译时间（如 "19:30:00"）
   * - ``_MSC_VER``
     - VC 版本号（VC10 = 1600）
   * - ``_DEBUG``
     - 调试模式下定义（/MTd 或 /MDd）
   * - ``_WIN32``
     - 32 位或 64 位 Windows 上定义
   * - ``_WIN64``
     - 仅 64 位编译时定义

# 运算符与 ## 运算符
=====================

字符串化（#）
--------------

.. code-block:: c

   #define PRINT_VAR(x)  printf(#x " = %d\n", x)

   int value = 42;
   PRINT_VAR(value);
   // 展开为: printf("value" " = %d\n", value);
   // 输出: value = 42

标记拼接（##）
---------------

.. code-block:: c

   #define MAKE_FUNC(name) \
       int func_##name(int arg) { \
           return arg * 2; \
       }

   MAKE_FUNC(add)    // 生成 int func_add(int arg) { ... }

   // 使用
   int result = func_add(10);    // 20

#pragma 指令
=============

.. code-block:: c

   // 消息输出（编译时）
   #pragma message("编译 " __FILE__)

   // 禁用特定警告
   #pragma warning(disable: 4996)       // 禁用 C4996（安全函数）
   #pragma warning(once: 4345)          // 警告只报一次
   #pragma warning(error: 4018)         // 将警告视为错误

   // 优化指令
   #pragma optimize("gt", on)           // 启用全局优化
   #pragma inline_depth(4)              // 内联深度

   // 内存对齐
   #pragma pack(push, 1)
   struct Packed { char a; int b; };
   #pragma pack(pop)

断言（assert）
===============

.. code-block:: c

   #include <assert.h>

   int divide(int a, int b) {
       // 调试时检查除数不为 0
       assert(b != 0);
       return a / b;
   }

   int main(void) {
       divide(10, 0);    // 断言失败：程序终止
       return 0;
   }

.. tip::

   - 断言在 ``#include <assert.h>`` 前定义 ``NDEBUG`` 即可关闭：
     ``#define NDEBUG``
   - VC10 的发布版配置默认定义 ``NDEBUG``，断言自动消失
   - 断言是调试工具，不要用来处理运行时错误（用 ``if`` 检查）
