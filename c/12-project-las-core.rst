=====================================
开发项目：LAS-CORE 测井数据解析库
=====================================

项目简介
=========

**LAS-CORE** 是一个从零编写的、仅依赖标准 C 运行库的 **LAS 文件格式解析库**，
用于解析测井行业通用的 Log ASCII Standard（LAS 2.0 / 3.0）数据文件。

项目与 :doc:`XTF-CORE <11-project-xtf-core>` 采用**同一套工程约定**（C89/C90、
make / CMake 双轨、零全局状态、GPL-3.0），在核心库之上同样提供
Fortran 2003 / Python 3 / C# .NET 3.5 / Lua 5.x 四套语言绑定，
是 C 语言「解析器 + 命令行工具 + 多语言绑定」综合项目的又一个完整范例。

.. list-table::
   :header-rows: 1

   * - 项目属性
     - 说明
   * - 项目名称
     - LAS-CORE
   * - 项目定位
     - LAS 2.0 / 3.0 测井数据文件（Log ASCII Standard）解析 / 生成库
   * - 语言标准
     - 纯 C（**C89/C90 / ANSI C**），兼容 VC10
   * - 依赖
     - 零第三方库，无 C++ 特性
   * - 许可证
     - GNU GPL v3.0
   * - 代码规模
     - 约 4,900 行（核心 4 模块 + C ABI 桥接层 + 4 种语言绑定 + 测试）

.. note::

   LAS-CORE 与 XTF-CORE 的关系：两者都是测井数据解析库，
   但**文件格式不同**——XTF 是 ECLIPS 5700 系统的二进制格式，
   LAS 是 CWLS 发布的行业标准文本交换格式。LAS-CORE 从 XTF-CORE
   复用工程基线（目录结构、测试框架、语言绑定方案），并额外支持
   **Lua 5.x** 绑定与 **2.0 <-> 3.0 双向版本转换**。

技术栈与设计约束
==================

核心技术
---------

- **语言**：纯 C89/C90，严格兼容 Microsoft Visual C++ 2010（VC10）
  （无 ``//`` 注释、无混合声明、无 ``long long``、无变长数组、无 ``stdint.h``；
  整型自行 typedef 为 ``las_i8/u8/i16/u16/i32/u32``，MSVC 下映射 ``__int8/16/32``）。
- **构建系统**：Makefile 与 CMake 双轨并行，产物一致。
- **编译门槛**：``gcc -std=c89 -pedantic -Wall -Wextra`` 零告警为提交门槛。
- **产物**：静态库 ``liblas.a``、共享库 ``bin/las.dll``，另有 Lua 模块
  ``bin/las_lua.dll``（静态链接核心库）与命令行示例 ``lasinfo``。

.. note::

   LAS-CORE 是本 DOCS 中 :doc:`C 语言基础 <index>` 章节的又一个**实战项目**：
   与 XTF-CORE 同属「解析器 + 命令行工具 + 多语言绑定」范式，
   但针对的是**文本格式**——文本解析无需字节序处理，核心难度在
   节头 / 助记符 / 分隔符的**容错**与 WRAP 换行的重建。

工程规范（硬性约定）
---------------------

- **无全局可变状态**：一切状态封装在句柄结构体（``LasFile``）内，通过指针传递。
- **公共接口最小化**：公共头 ``include/las.h`` 只暴露只读数据结构与不透明句柄；
  内部布局定义在私有头 ``src/las_internal.h``。
- **统一错误处理**：所有函数返回 ``LasError`` 枚举，配合 ``las_error_str()`` 转可读文本；
  禁止裸 ``exit()``；新增错误码**追加**到枚举末尾，避免数值漂移。
- **字符串边界检查**：字符串复制使用 ``las_copy_seg``，避免 ``strcpy`` 溢出。
- **每个公开 API 函数都有单元测试**：核心 ``tests/test_las.c`` 共 180 项断言，
  桥接层 ``tests/test_bridge.c`` 共 106 项，全部通过。

系统架构
=========

目录结构
---------

.. code-block::

   include/las.h         公共接口（只读数据 + 不透明句柄 + 函数原型）
   src/las_internal.h    内部结构体（不对外暴露）
   src/las_util.c        行裁剪、大小写不敏感比较、数值扫描、~Curve 行解析
   src/las_core.c        读 LAS：las_open/las_close、节解析、数据累积
   src/las_access.c      读 LAS：信息访问、按名称查找、按列读数据
   src/las_write.c       写 LAS：las_export_version（2.0/3.0）
   src/api/              语言绑定（统一 C ABI 桥接层）
     las_bridge.{c,h}    C ABI 桥接层（LasHandle = void*）
     fortran2003/        Fortran 2003 绑定（bind(C)）+ 测试
     python3/            Python 3 绑定（ctypes）+ 测试
     dotnet/             C# .NET 3.5 绑定（P/Invoke）+ 测试
     lua/                Lua 5.x 绑定（las_lua.c）+ 测试
     build_all.sh        一键构建并运行四种语言测试
   tests/test_las.c      单元测试（180 项断言）
   tests/test_bridge.c   C ABI 桥接层单元测试（106 项断言）
   examples/lasinfo.c    命令行示例（读 LAS / 打印信息 / 导出转换）
   LAS/                  LAS 2.0 / 3.0 样例数据文件（只读）
   docs/                 格式规范 + 开发文档
   bin/ work/            构建产物与测试工作目录

分层设计
---------

.. code-block::

   ┌─────────────────────────────────────────────────────┐
   │  语言绑定层  Fortran / Python / C# / Lua 5.x         │
   ├─────────────────────────────────────────────────────┤
   │  C ABI 桥接层  src/api/las_bridge.c（句柄 void*）     │
   ├─────────────────────────────────────────────────────┤
   │  核心实现层  src/*.c（4 个模块，C89 零依赖）          │
   ├─────────────────────────────────────────────────────┤
   │  公共接口层  include/las.h（不透明句柄 + 只读结构体）  │
   └─────────────────────────────────────────────────────┘

核心功能
=========

读 LAS（las_core / las_access）
-------------------------------

- 入口 ``las_open``：自动识别 LAS 2.0 / 3.0 版本；
- 解析 ``~Version`` / ``~Well`` / ``~Curve`` / ``~Parameter`` / ``~A`` 五类节；
- **容错解析**：节头兼容完整名（``~Curve``）与单字母缩写（``~C``），大小写不敏感；
  助记符兼容点分（``DEPT.M``）与空格分隔（``GR GAPI``）；数据兼容逗号/空白分隔，
  WRAP YES/NO 均按 token 数重建采样行；
- ``las_read_curve`` 按列读取数据，NULL 占位值（如 -999.25）原样保留。

导出与版本转换（las_write）
----------------------------

- ``las_export_version(las, out, 2|3)`` 将已解析内容按目标版本重新导出，
  实现 **LAS 2.0 <-> 3.0 双向转换**；
- ``las_export`` 等价于导出为 2.0。

语言绑定（src/api）
--------------------

在核心库之上提供**统一 C ABI 桥接层**，供四种语言调用共享库 ``bin/las.dll``：

.. list-table::
   :header-rows: 1

   * - 语言
     - 绑定方式
     - 测试
   * - Fortran 2003
     - ``bind(C)`` 接口模块（las_interface.f90）
     - 46 项断言
   * - Python 3
     - ``ctypes.CDLL``，提供 ``LasFile`` 便捷类
     - 42 项断言
   * - C# .NET 3.5
     - ``[DllImport]`` P/Invoke（LasApi.cs）
     - 40 项断言
   * - Lua 5.x
     - Lua C 模块（las_lua.c，``luaopen_las`` 导出）
     - 45 项断言

.. tip::

   Lua 绑定是 LAS-CORE 相对 XTF-CORE 的增量，值得单独说明：
   ``las_lua.c`` 直接以 C 模块编译为 ``bin/las_lua.dll``，
   错误以 Lua ``error`` 抛出 ``{ code, msg }`` 表，可用 ``pcall`` 捕获；
   系统无 Lua 时构建脚本会自动下载源码自举到 ``bin/lua54/``。

示例工具
---------

.. code-block::

   lasinfo 文件.las [输出.las [2|3]]   # 读 LAS 打印信息；给输出则按版本导出

LAS 格式要点
==============

格式规范见 ``docs/LAS_FORMAT.md``，要点速记：

- LAS 是「节式」**文本**文件，节头以 ``~`` 起始；必需节为 ``~Version`` / ``~Well`` /
  ``~Curve`` / ``~A``，可选 ``~Parameter`` / ``~Other``。
- ``~Version``：``VERS.`` 版本号（2.0 / 3.0，必须）、``WRAP.`` 换行方式。
- ``~Curve`` 每行定义一列：``MNEM.UNIT : DESC``；LAS 2.0 助记符 4 字符，
  LAS 3.0 最长 8 字符。
- ``~A`` 逐采样行排列，每行 ``curve_count`` 个值（深度 + 各曲线值），
  逗号或空白分隔；**WRAP YES** 时一个采样行可跨多个物理行。
- 解析策略：**按 token 流累积**，每 ``curve_count`` 个 token 构成一行，
  与物理行边界无关，从而天然支持 WRAP YES/NO。
- NULL 占位值（如 -999.25）原样保留；曲线数据按「行主序」存储：
  ``data[row * curve_count + col]``。
- 文本格式**无字节序问题**，数值一律经 ``strtod`` / ``atof`` 解析；
  文件可有 UTF-8 BOM，行结束符兼容 ``\n`` 与 ``\r\n``。

开发记录
=========

项目于 2026-08-10 完成「核心解析库 → 语言绑定层」的完整交付，
git 单次提交 ``18ed3ab``，共 37 文件、+7578 行；CHANGELOG 分两个里程碑：

.. list-table::
   :header-rows: 1

   * - 里程碑
     - 内容
   * - 0.1.0（核心解析库）
     - ``las_open`` / 节解析 / 容错 / 数据访问 / ``las_export_version``
       导出 / ``test_las.c`` 180 断言 / ``lasinfo`` / 样例数据 / 文档
   * - 0.2.0（语言绑定层）
     - 统一 C ABI 桥接层、Fortran / Python / C# / Lua 四套绑定及测试
       （46 / 42 / 40 / 45 断言）、``test_bridge.c`` 106 断言、
       ``las_shared`` 共享库、``build_all.sh`` 一键脚本

工程实践要点
-------------

- **复用 XTF-CORE 工程基线**：目录结构、测试框架、语言绑定方案直接沿用，
  新项目只聚焦「LAS 文本格式」本身的差异，大幅降低工程成本。
- **容错优先**：节头 / 助记符 / 分隔的多种写法一律容忍，优先保证「能读出真实数据」。
- **测试先行**：每个公开 API 函数均有断言（含错误路径）；
  断言数由 179 更正为 180（实测计数），数据以实测为准。
- **工具链踩坑**：本机 ``gfortran`` 损坏（trivial 程序也静默退出 1），
  Fortran 绑定必须用 **flang-new**；PATH 上的 ``python3`` 是 Microsoft Store 存根，
  各测试脚本自动探测 clang64 的 python；MSYS 环境丢弃系统 ``TMP``，
  构建脚本在仓库内自建 ``.build_tmp``；无 Lua 环境时自动下载源码自举。

构建与测试
===========

.. code-block:: bash

   # 方式一：make
   make            # 编译 liblas.a + 测试 + 示例
   make test       # 运行核心单元测试（180 断言）
   make test_all   # 核心 + 桥接层（106 断言）

   # 方式二：CMake（产物输出 bin/，CTest 在 work/ 运行）
   cmake -S . -B build
   cmake --build build
   cmake --build build --target las_shared   # 共享库 bin/las.dll
   ctest --test-dir build

   # 语言绑定（先构建共享库，一键跑四种语言测试）
   sh src/api/build_all.sh

快速使用
=========

.. code-block:: c
   :caption: 读取 LAS 并按名称读取一条曲线，再转为 3.0 导出

   #include "las.h"

   LasFile *las = 0;
   LasError e = las_open("LAS/sample20.las", &las);
   if (e != LAS_OK) {
       fprintf(stderr, "open failed: %s\n", las_error_str(e));
       return 1;
   }

   printf("版本: %.1f  曲线: %d  采样: %d\n",
          las_version(las), las_curve_count(las), las_sample_count(las));

   int idx = las_curve_index_by_name(las, "GR");   /* 按名称查索引 */
   long n = las_sample_count(las);
   float *buf = (float *)malloc((size_t)n * sizeof(float));
   long got = las_read_curve(las, idx, 0, n, buf);

   printf("%s: %.4f ...\n", las_curve_name(las, idx), buf[0]);

   FILE *out = fopen("out.las", "wb");
   las_export_version(las, out, LAS_VERSION_3);     /* 转 LAS 3.0 */
   fclose(out);

   las_close(las);

相关链接
=========

- 仓库：``D:\HJR\LAS-CORE``
- 格式规范：``docs/LAS_FORMAT.md``
- 开发文档：``docs/DEVELOPMENT.md``
- 变更日志：``CHANGELOG.md``
- 样例数据：``LAS/sample20.las``，LAS 2.0；``LAS/sample30.las``，LAS 3.0（WRAP YES）
- 姊妹项目：:doc:`XTF-CORE <11-project-xtf-core>`，ECLIPS 5700 二进制格式解析库
