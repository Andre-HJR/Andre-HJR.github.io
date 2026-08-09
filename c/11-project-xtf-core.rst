=====================================
开发项目：XTF-CORE 测井文件解析库
=====================================

项目简介
=========

**XTF-CORE** 是一个从零编写的、仅依赖标准 C 运行库的 **XTF 文件格式解析与生成库**，
用于解析 Atlas 公司 ECLIPS 5700 数控测井系统 / eXpress 解释软件生成的 XTF 二进制数据文件。

项目将专有的 XTF 测井数据格式（经逆向工程整理出规范）解析为通用结构，
并支持 LAS 2.0 / 3.0（测井行业标准文本格式）的双向转换，
是 C 语言经典「解析器 + 命令行工具 + 多语言绑定」综合项目的完整范例。

.. list-table::
   :header-rows: 1

   * - 项目属性
     - 说明
   * - 项目名称
     - XTF-CORE
   * - 项目定位
     - ECLIPS 5700 测井系统 XTF 文件解析 / 生成库（附 LAS 双向转换）
   * - 语言标准
     - 纯 C（**C89/C90 / ANSI C**），兼容 VC10
   * - 依赖
     - 零第三方库，无 C++ 特性
   * - 许可证
     - GNU GPL v3.0
   * - 代码规模
     - 约 7,100 行（核心 7 个模块 + C ABI 桥接层 + 3 种语言绑定 + 测试）

技术栈与设计约束
==================

核心技术
---------

- **语言**：纯 C89/C90，严格兼容 Microsoft Visual C++ 2010（VC10）对 C 的支持
  （无 ``//`` 注释、无混合声明、无 ``long long``、无变长数组、无 ``stdint.h``；
  整型自行 typedef 为 ``xtf_i8/u8/i16/u16/i32/u32``，MSVC 下映射 ``__int8/16/32``）。
- **构建系统**：Makefile 与 CMake 双轨并行，产物一致。
- **编译门槛**：``gcc -std=c89 -pedantic -Wall -Wextra`` 零告警为提交门槛。
- **产物**：静态库 ``libxtf.a``、共享库 ``xtf.dll``，供语言绑定调用，另有三个示例工具、一个测试程序。

.. note::

   XTF-CORE 是本 DOCS 中 :doc:`C 语言基础 <index>` 章节的**实战项目**：
   将前 10 节的语法、指针、结构体、文件 I/O、预处理器等知识，
   应用于一个生产级的二进制格式解析库，并严格约束在 C89 子集内编写。

工程规范（硬性约定）
---------------------

- **无全局可变状态**：一切状态封装在句柄结构体（``XtfFile`` / ``XtfWriter`` / ``LasFile``）内，
  通过指针传递，禁止隐式共享全局缓冲。
- **公共接口最小化**：公共头 ``include/xtf.h`` 只暴露只读数据结构与不透明句柄；
  内部布局定义在私有头 ``src/xtf_internal.h``，不泄漏到公共接口。
- **统一错误处理**：所有函数返回 ``XtfError`` 枚举，配合 ``xtf_error_str()`` 转可读文本；
  禁止裸 ``exit()``。
- **字符串边界检查**：定长字段复制使用 ``xtf_trim_field``，避免 ``strcpy`` 溢出。
- **每个公开 API 函数都有单元测试**：``tests/test_xtf.c`` 共 415 项断言，全部通过。

系统架构
=========

目录结构
---------

.. code-block::

   include/xtf.h         公共接口（只读数据 + 不透明句柄 + 函数原型）
   src/xtf_internal.h    内部结构体（不对外暴露）
   src/xtf_util.c        字节序与字符串工具
   src/xtf_core.c        读 XTF：打开/关闭、标题记录 1~8、曲线头解析
   src/xtf_curve.c       读 XTF：数据类型表、曲线数据读取
   src/xtf_access.c      读 XTF：信息访问、按名称查找
   src/xtf_write.c       写 XTF：xtf_writer_*
   src/xtf_las.c         写 LAS：xtf_export_las_version
   src/xtf_lasread.c     读 LAS：xtf_las_* 与 LAS→XTF 转换
   src/api/              语言绑定（统一 C ABI 桥接层）
     xtf_bridge.{c,h}    C ABI 桥接层
     fortran2003/        Fortran 2003 绑定（bind(C)）
     python3/            Python 3 绑定（ctypes）
     dotnet/             C# .NET 3.5 绑定（P/Invoke）
   tests/test_xtf.c      单元测试（415 项断言）
   examples/             示例工具（xtf_dump / xtf2las / lasinfo）
   XTF/                  实测数据文件（只读）
   docs/                 格式规范 + 开发文档 + 参考 PDF

分层设计
---------

.. code-block::

   ┌─────────────────────────────────────────────────────┐
   │  语言绑定层  Fortran 2003 / Python 3 / C# .NET 3.5   │
   ├─────────────────────────────────────────────────────┤
   │  C ABI 桥接层  src/api/xtf_bridge.c（句柄 void*）     │
   ├─────────────────────────────────────────────────────┤
   │  核心实现层  src/*.c（7 个模块，C89 零依赖）          │
   ├─────────────────────────────────────────────────────┤
   │  公共接口层  include/xtf.h（不透明句柄 + 只读结构体）  │
   └─────────────────────────────────────────────────────┘

核心功能
=========

读 XTF（xtf_core / xtf_curve）
-------------------------------

- 入口 ``xtf_open``：读取 8 条标题记录，校验 ``WSI`` 魔数；
- **字节序自动检测**：以标题记录 1 的记录位置标记自动判别大端 / 小端，
  所有整数与浮点按检测结果读取（实测样例为 Sun 书写的大端文件）；
- **偏移约定容错**：曲线头起始记录号可能是记录号约定或字节偏移约定，
  以曲线头名称与标题记录 3 比对自动判别；
- 支持 12 种数据类型解码：REAL\*4 / REAL\*8 / INT\*2 / INT\*4 / BIT（位打包）、
  12 位打包（I12S / I12U）等；
- ``xtf_read_curve_float`` / ``xtf_read_curve_raw`` 读取曲线数据。

写 XTF（xtf_write）
--------------------

- ``xtf_writer_*`` 系列：创建 → 追加曲线 → 收尾落盘，全程大端；
- 支持数据类型 2/3/4/7/8/9/10/11，曲线数上限 512；
- 收尾时一次性写出 8 条标题记录（分配位图、曲线名、交错索引、井场信息等）。

LAS 双向转换（xtf_las / xtf_lasread）
--------------------------------------

- ``xtf_export_las_version`` 导出 LAS **2.0 / 3.0**；深度为合成索引，
  多元素曲线拆分多列）；
- ``xtf_las_*`` 读取 LAS：**自动识别版本**，助记符兼容点分与空格两种写法、
  WRAP YES/NO、逗号/空白分隔；
- ``xtf_las_to_xtf`` 将 LAS 转回 XTF（列数 > 512 返回 ``XTF_ERR_BAD_CURVE_COUNT``）。

语言绑定（src/api）
--------------------

在核心库之上提供**统一 C ABI 桥接层**，供三种语言调用共享库 ``xtf.dll``：

.. list-table::
   :header-rows: 1

   * - 语言
     - 绑定方式
     - 测试
   * - Fortran 2003
     - ``bind(C)`` 接口模块（iso_c_binding）
     - 31 项断言
   * - Python 3
     - ``ctypes.CDLL``，提供 ``XtfFile / LasFile / XtfWriter`` 便捷类
     - 27 项断言
   * - C# .NET 3.5
     - ``[DllImport]`` P/Invoke
     - 23 项断言

示例工具
---------

.. code-block::

   xtf_dump 文件.xtf [索引|曲线名]     # 打印文件信息与指定曲线数据
   xtf2las  文件.xtf [输出.las] [2|3]  # XTF → LAS（默认 2.0）
   lasinfo  文件.las [输出.xtf]        # 读 LAS；给第二个参数则转回 XTF

XTF 格式要点
==============

逆向整理的格式规范见 ``docs/XTF_FORMAT.md``，要点速记：

- **记录 = 4096 字节**；标题块 = 8 条记录（32768 字节）。
- 标题记录 1：实际曲线数（ISNUMCV）在偏移 996，ISMAXCV 在 1000。
- 标题记录 3：512 × 8 字节曲线名。
- 标题记录 4：前 2048 字节为各曲线起始记录号（**交错存放**：曲线 1, 257, 2, 258, …），
  后 2048 字节为采样数。
- 标题记录 7：前 2048 字节为采样间距，后 2048 字节为 4 字节类型码。
- 每条曲线 = 1 条曲线头记录 + N 条数据记录；数据偏移 = 起始记录号 × 4096。
- 曲线数据总元素数 = 采样数 × 各维单元数之积；数据类型 4 = REAL\*4（4 字节）。

开发记录
=========

项目在 2026-08-08 一天内完成了「逆向 → 实现 → 测试 → 多语言封装」的完整交付，
共 3 次提交：

.. list-table::
   :header-rows: 1

   * - 提交
     - 内容
   * - ``96dec85``
     - 初始提交：XTF/LAS 解析库（C89/C90）与 Fortran/Python/C# 语言绑定
   * - ``34067aa``
     - 合并 DOC/ 与 docs/ 为一个目录，更新相关文档引用
   * - ``ccb83f4``
     - 采用 GNU GPL v3.0 许可：新增 LICENSE、全部源码加 GPL-3.0 头注释

工程实践要点
-------------

- **逆向优先**：先整理实测文件与参考 PDF 形成 ``XTF_FORMAT.md`` 格式规范，
  再按规范实现，避免边写边猜。
- **容错设计**：字节序自动检测、偏移约定自动判别，使库能解析不同书写器的文件。
- **测试先行**：每个公开函数均有断言；写 → 读回环、LAS 2.0/3.0 双向、越界与非法参数路径全覆盖。
- **可移植性**：以 C89 子集严格约束，同时兼容 GCC/Clang 与 VC10，并有 make / CMake 双构建轨。

构建与测试
===========

.. code-block:: bash

   # 方式一：make
   make            # 编译 libxtf.a + 测试 + 示例
   make test       # 运行单元测试

   # 方式二：CMake
   cmake -S . -B build
   cmake --build build
   ctest --test-dir build       # 测试工作目录为 work/

   # 语言绑定（先构建共享库）
   cmake --build build --target xtf_shared
   sh src/api/build_all.sh

快速使用
=========

.. code-block:: c
   :caption: 读取 XTF 并按名称读取一条曲线

   #include "xtf.h"

   XtfFile *f = 0;
   XtfError e = xtf_open("SHAYAN_CAL_20250812.xtf", &f);
   if (e != XTF_OK) {
       fprintf(stderr, "open failed: %s\n", xtf_error_str(e));
       return 1;
   }

   int idx = xtf_curve_index_by_name(f, "AMD1F1");   /* 按名称查索引 */
   long total = xtf_curve_element_count(f, idx);

   float *buf = malloc((size_t)total * sizeof(float));
   long got = xtf_read_curve_float(f, idx, 0, total, buf);

   printf("%s: %.4f ...\n", xtf_curve_name(f, idx), buf[0]);
   xtf_close(f);

相关链接
=========

- 仓库：``D:\HJR\XTF-CORE``
- 格式规范：``docs/XTF_FORMAT.md``
- 开发文档：``docs/DEVELOPMENT.md``
- 实测样例：``XTF/SHAYAN_CAL_20250812.xtf``，只读
