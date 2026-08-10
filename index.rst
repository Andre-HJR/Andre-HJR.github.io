HJR Docs 技术文档
====================

欢迎来到 HJR Docs！本文档系统整理了编程语言基础与机器学习理论，
从环境搭建到核心概念，帮助开发者快速上手并深入理解各项技术。

编程语言
----------

.. grid:: 2

   .. grid-item-card:: 🔧 C 语言基础
      :link: c/index
      :link-type: doc

      高效、灵活的底层编程语言
      ^^^^^^^^^^^^^^^^^^^^^^^^
      C 语言是操作系统、嵌入式和高性能应用的首选，
      VC10（Visual C++ 2010）是 Windows 平台经典的开发环境。

      +++
      **内容涵盖：** 语法基础 · 指针 · 结构体 · 文件 I/O · 预处理器 · VC10 调试 · 项目实践（XTF-CORE / LAS-CORE）

   .. grid-item-card:: 📐 Fortran 基础
      :link: fortran/index
      :link-type: doc

      科学计算领域的常青树
      ^^^^^^^^^^^^^^^^^^^^
      Fortran 是历史最悠久的高级语言之一，在数值计算和
      高性能计算领域仍不可或缺。IVF 2011 是 Intel 的旗舰编译器。

      +++
      **内容涵盖：** 语法基础 · 数组 · 模块 · 文件 I/O · MKL · OpenMP

   .. grid-item-card:: 🐘 Scala 基础
      :link: scala/index
      :link-type: doc

      函数式与面向对象融合的 JVM 语言
      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
      Scala 融合了面向对象与函数式编程范式，运行于 JVM 之上，
      是大数据处理（Apache Spark）和微服务领域的主流选择。

      +++
      **内容涵盖：** 语法基础 · OOP · 函数式编程 · 集合框架 · 类型系统 · 并发编程

   .. grid-item-card:: 📱 Flutter 基础
      :link: flutter/index
      :link-type: doc

      跨平台移动与桌面 UI 框架
      ^^^^^^^^^^^^^^^^^^^^^^^^
      Flutter 是 Google 开源的 UI 工具包，一套代码即可构建
      移动端、Web 端和桌面端应用。基于 Dart 语言和自绘引擎。

      +++
      **内容涵盖：** Dart 语言 · Widget 体系 · 布局 · 状态管理 · 路由 · 网络 · 平台集成 · 项目实践（蓝牙 MCU 控制器）

   .. grid-item-card:: 🎯 C# 编程基础
      :link: csharp/index
      :link-type: doc

      .NET 平台的主力语言
      ^^^^^^^^^^^^^^^^^^^
      C# 是 .NET 生态的核心语言，从桌面到 Web 到云原生全面覆盖。
      从基础语法到 WinForms/WPF 桌面开发，系统学习 C# 编程。

      +++
      **内容涵盖：** 语言基础 · OOP · LINQ · async/await · WinForms · WPF + MVVM · EF Core

   .. grid-item-card:: 🛠️ 开发项目
      :link: projects/index
      :link-type: doc

      亲手完成的项目展示
      ^^^^^^^^^^^^^^^^^^^
      汇总 HJR 的独立开发实践：测井文件解析库与 BLE 蓝牙控制器，
      附架构图、开发记录与踩坑复盘。

      +++
      **项目：** XTF-CORE 测井解析库 · 蓝牙 MCU 控制器

统计与机器学习
----------------

.. grid:: 2

   .. grid-item-card:: 📊 半监督学习
      :link: statistics/ssl/index
      :link-type: doc

      统计视角下的半监督学习
      ^^^^^^^^^^^^^^^^^^^^^^
      半监督学习旨在利用少量有标签数据和大量无标签数据构建更好的分类器。
      从统计缺失数据框架出发，系统梳理 SSL 的理论与方法。

      +++
      **内容涵盖：** 缺失标签机制 · 核心假设 · 自训练/EM · Fisher 信息 · 一致性正则化

硬件与射频工程
---------------

.. grid:: 2

   .. grid-item-card:: 📡 射频工程基础
      :link: rf/index
      :link-type: doc

      射频电路与系统设计
      ^^^^^^^^^^^^^^^^^^
      从传输线理论到 S 参数、阻抗匹配、PCB 布局，系统掌握射频工程
      的核心知识与实践技能。包含 ADS 仿真思路和完整设计实例。

      +++
      **内容涵盖：** 传输线 · S 参数 · 史密斯圆图 · 匹配网络 · PCB 设计 · ADS 仿真 · 测量

   .. grid-item-card:: 📶 ESP32 嵌入式开发
      :link: esp32/index
      :link-type: doc

      WiFi/BLE 无线 MCU 开发
      ^^^^^^^^^^^^^^^^^^^^^^^
      乐鑫 ESP32 系列是物联网领域最广泛使用的无线 MCU。
      从 ESP-IDF 到 MicroPython，系统掌握 ESP32 开发。

      +++
      **内容涵盖：** ESP-IDF · MicroPython · FreeRTOS · WiFi/BLE · 外设驱动 · 调试

   .. grid-item-card:: ⚡ DSP TMS320F28335
      :link: dsp/index
      :link-type: doc

      数字信号处理器开发
      ^^^^^^^^^^^^^^^^^^
      TI C2000 系列 DSP 是数字电源、电机控制和逆变器的核心芯片。
      从 CCS 环境到 ePWM/ADC 外设，掌握工业级 DSP 开发。

      +++
      **内容涵盖：** CCS IDE · 系统控制 · ePWM · ADC · eQEP · CAN · Flash 编程

.. toctree::
   :maxdepth: 2
   :caption: ⚙️ C 语言
   :numbered:

   c/index

.. toctree::
   :maxdepth: 2
   :caption: 📐 Fortran
   :numbered:

   fortran/index

.. toctree::
   :maxdepth: 2
   :caption: 🐘 Scala
   :numbered:

   scala/index

.. toctree::
   :maxdepth: 2
   :caption: 📱 Flutter
   :numbered:

   flutter/index

.. toctree::
   :maxdepth: 2
   :caption: 🎯 C#
   :numbered:

   csharp/index

.. toctree::
   :maxdepth: 2
   :caption: 🛠️ 开发项目
   :numbered:

   projects/index

.. toctree::
   :maxdepth: 2
   :caption: 📊 统计与机器学习
   :numbered:

   statistics/ssl/index

.. toctree::
   :maxdepth: 2
   :caption: 📡 射频工程
   :numbered:

   rf/index

.. toctree::
   :maxdepth: 2
   :caption: 📶 ESP32
   :numbered:

   esp32/index

.. toctree::
   :maxdepth: 2
   :caption: ⚡ DSP F28335
   :numbered:

   dsp/index

--------------

.. admonition:: 使用说明
   :class: note

   - 本文档使用 **Sphinx** 构建，支持 reStructuredText 和 Markdown (MyST) 两种格式。
   - 代码块右上角的 📋 按钮可一键复制代码。
   - 使用左侧边栏的搜索框 🔍 快速定位内容。
   - 本项目的配置文件位于 :download:`conf.py`。
