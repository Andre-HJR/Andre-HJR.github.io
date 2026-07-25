=========================
VC10 特有功能与调试
=========================

本章介绍 Visual C++ 2010 特有的编译选项、链接器配置和调试技术。

VC10 编译器选项
=================

常用编译选项
-------------

.. list-table::
   :header-rows: 1

   * - 选项
     - 说明
   * - ``/c``
     - 仅编译，不链接
   * - ``/Fo``
     - 指定目标文件 (.obj) 输出路径
   * - ``/Fe``
     - 指定可执行文件 (.exe) 输出路径
   * - ``/I``
     - 添加头文件搜索路径
   * - ``/D``
     - 定义预处理器宏
   * - ``/EHsc``
     - 启用 C++ 异常处理
   * - ``/W3`` / ``/W4``
     - 警告级别（推荐 ``/W4`` 以提高代码质量）
   * - ``/WX``
     - 将警告视为错误
   * - ``/Zi``
     - 生成调试信息（PDB 文件）
   * - ``/O2``
     - 速度优化（发布版）
   * - ``/Od``
     - 禁用优化（调试版）
   * - ``/MT``
     - 静态链接运行时库（发布版）
   * - ``/MD``
     - 动态链接运行时库（发布版 DLL）
   * - ``/MTd``
     - 静态链接调试运行时库（调试版）
   * - ``/MDd``
     - 动态链接调试运行时库（调试版 DLL）

运行时库选择
-------------

.. code-block:: text

   项目属性 → 配置属性 → C/C++ → 代码生成 → 运行时库

   /MT    = 多线程静态库（发布版，不需 VC 运行库 DLL）
   /MTd   = 多线程静态库（调试版）
   /MD    = 多线程动态 DLL（发布版，需 msvcr100.dll）
   /MDd   = 多线程动态 DLL（调试版，需 msvcr100d.dll）

.. important::

   确保项目中所有模块使用**相同**的运行时库。
   混用 ``/MT`` 和 ``/MD`` 会导致链接错误。

预编译头
=========

VC10 使用预编译头（PCH）加速编译：

.. code-block:: c
   :caption: stdafx.h

   // stdafx.h：预编译头文件（包含稳定的系统头文件）
   #pragma once

   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include <tchar.h>
   #include <windows.h>

   // 在此添加常用头文件

.. code-block:: c
   :caption: stdafx.cpp

   // stdafx.cpp：预编译头实现文件
   #include "stdafx.h"

配置步骤：

.. code-block:: text

   1. 项目 → 属性 → C/C++ → 预编译头
   2. 预编译头 = "使用 (/Yu)"
   3. 预编译头文件 = "stdafx.h"
   4. 对 stdafx.cpp：预编译头 = "创建 (/Yc)"

.. tip::

   将不常修改的系统头文件和第三方库头文件放入 ``stdafx.h``，
   可大幅减少每次编译的时间。

链接器配置
===========

.. code-block:: text

   项目 → 属性 → 链接器 → 输入 → 附加依赖项

   // 常见 Windows 库
   kernel32.lib      // 核心系统函数
   user32.lib        // Windows 界面
   gdi32.lib         // 图形设备接口
   ws2_32.lib        // Winsock 网络
   comctl32.lib      // 通用控件

   // 链接特定函数导出
   #pragma comment(lib, "winmm.lib")     // 多媒体
   #pragma comment(lib, "ws2_32.lib")    // 网络
   #pragma comment(linker, "/STACK:4194304")  // 修改栈大小（4MB）

调试技术
=========

断点
-----

.. code-block:: text

   F9           切换断点
   Ctrl + F9    禁用断点
   Alt + F9     新建数据断点（内存地址变化时中断）

监视窗口
----------

.. code-block:: text

   调试 → 窗口 → 监视 (Ctrl + Alt + W, 1-4)

   常用监视表达式：
   arr, 5         查看数组前 5 个元素
   p             查看指针指向的值
   (type)expr    强制类型转换后查看
   errno, hr     查看错误码和 HRESULT

调用堆栈
----------

.. code-block:: text

   调试 → 窗口 → 调用堆栈 (Ctrl + Alt + C)

   - 双击任意栈帧可跳到对应代码
   - 右键 → "显示源代码" 快速定位
   - 右键 → "显示反汇编" 查看汇编

内存窗口
----------

.. code-block:: text

   调试 → 窗口 → 内存 (Ctrl + Alt + M, 1-4)

   地址栏输入 &variable 即可查看变量内存
   右键可选择显示格式（字节/字/四字/浮点）

数据断点
---------

.. code-block:: text

   调试 → 新建断点 → 新建数据断点

   在地址栏输入 &variable，指定字节数
   当该内存区域被修改时自动中断

.. tip::

   数据断点对查找"谁偷偷修改了这个变量"非常有用。

VC10 调试宏与函数
==================

.. code-block:: c

   #include <crtdbg.h>

   // 断言
   _ASSERT(ptr != NULL);            // 调试版本有效
   _ASSERTE(ptr != NULL);           // 显示表达式文本

   // 验证（发布版也有效）
   _ASSERT_EXPR(ptr != NULL, L"指针为空！");

   // 运行时检查
   int result = _CrtDbgReport(
       _CRT_ASSERT,                 // 报告类型
       __FILE__,
       __LINE__,
       NULL,
       "错误代码: %d\n", error_code
   );

内存泄漏检测
-------------

.. code-block:: c
   :caption: main.c

   #define _CRTDBG_MAP_ALLOC
   #include <stdlib.h>
   #include <crtdbg.h>

   int main(void) {
       // 启用内存泄漏检测（程序退出时自动报告）
       _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |
                       _CRTDBG_LEAK_CHECK_DF);

       int *p = (int *)malloc(100);
       // 故意忘记释放——调试输出窗口会报告泄漏

       return 0;
   }

.. code-block:: text

   输出窗口会显示：
   Detected memory leaks!
   Dumping objects ->
   {45} normal block at 0x005A3F90, 100 bytes long.
   Data: <                > CD CD CD CD CD CD ...

   // 然后在代码中设置断点等待第 45 次分配：
   _CrtSetBreakAlloc(45);

常见编译错误与解决
===================

.. list-table::
   :header-rows: 1

   * - 错误信息
     - 原因
     - 解决
   * - C4996: 'strcpy' was declared deprecated
     - 安全函数警告
     - 用 ``strcpy_s`` 或定义 ``_CRT_SECURE_NO_WARNINGS``
   * - LNK2019: unresolved external symbol
     - 未解析的外部符号
     - 检查库依赖、函数声明是否匹配
   * - LNK2001: unresolved external symbol _main
     - 缺少 main 函数
     - 检查入口函数名（WinMain / main）
   * - C2065: undeclared identifier
     - 未声明标识符
     - 变量在开头声明（C89），或加头文件
   * - C2143: syntax error : missing ';'
     - 缺少分号
     - 检查前一行结构体/枚举定义
   * - C4013: function undeclared
     - 函数未声明
     - 加函数原型或 ``#include`` 头文件
   * - LNK1123: failure during conversion to COFF
     - 文件损坏或工具链问题
     - 升级至 SP1 或换用较新 VS

项目配置速查
=============

.. code-block:: text

   配置属性 → 常规
      字符集: 使用 Unicode 字符集 / 使用多字节字符集
      配置类型: 应用程序(.exe) / 静态库(.lib) / DLL(.dll)

   配置属性 → C/C++
      优化: 禁用(/Od) / 最小空间(/O1) / 最快速度(/O2)
      调试信息格式: 程序数据库(/Zi)
      警告等级: Level 3(/W3) / Level 4(/W4)
      将警告视为错误: 是(/WX)

   配置属性 → 链接器
      子系统: 控制台 / Windows
      入口点: main / WinMain / DllMain
      堆栈保留大小: 1048576 (1MB)  / 4194304 (4MB)

.. note::

   **Debug 与 Release 配置的关键差异：**

   +------------------+----------------------------+-------------------------+
   | 设置             | Debug                      | Release                 |
   +==================+============================+=========================+
   | 优化             | 禁用 (/Od)                 | 速度优化 (/O2)          |
   +------------------+----------------------------+-------------------------+
   | 调试信息         | 完整 (/Zi)                 | 无或 PDB                |
   +------------------+----------------------------+-------------------------+
   | 运行时库         | 多线程调试 (/MTd)          | 多线程 (/MT)            |
   +------------------+----------------------------+-------------------------+
   | _DEBUG           | 定义                       | 未定义                  |
   +------------------+----------------------------+-------------------------+
   | 断言行为         | 生效                       | 被忽略                  |
   +------------------+----------------------------+-------------------------+
