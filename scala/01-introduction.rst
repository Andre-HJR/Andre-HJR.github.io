=========================
Scala 简介与环境搭建
=========================

Scala 语言概述
===============

Scala（**Scal**\ able **La**\ nguage）是一门运行在 JVM 上的多范式编程语言，
由 Martin Odersky 于 2004 年发布。它将 **面向对象编程** 与 **函数式编程**
融合在一起，旨在以简洁、优雅的方式表达通用编程模式。

核心特性
---------

- **双范式融合**：类是纯粹的 OOP，函数是一等公民的 FP
- **静态强类型**：借助类型推断，代码简洁且安全
- **表达力强**：高阶函数、模式匹配、隐式转换等高级特性
- **与 Java 互操作**：直接调用 Java 库，运行在 JVM 上
- **并发友好**：Actor 模型（Akka）、Future、异步编程

典型应用场景
-------------

- 大数据平台：Apache Spark、Apache Flink 的核心语言
- 高并发后端：Akka HTTP、Play Framework、http4s
- 数据工程与 ETL：类型安全的批处理与流处理
- 函数式架构：结合 cats / ZIO 构建纯函数式系统

环境搭建
=========

安装 JDK
---------

Scala 需要 JDK 11 或更高版本：

.. code-block:: bash

   # 验证 JDK
   java -version

   # 建议使用 Adoptium (Eclipse Temurin)
   # https://adoptium.net/

.. tip::

   推荐使用 JDK 17 LTS 或 JDK 21 LTS，兼顾稳定性和新特性。

安装 SBT（Scala 标准构建工具）
--------------------------------

SBT（Scalable Build Tool）是 Scala 生态中最常用的构建工具。

**Windows**

.. code-block:: bash

   # 通过 scoop 安装
   scoop install sbt

   # 或下载 MSI 安装包
   # https://www.scala-sbt.org/download.html

**macOS**

.. code-block:: bash

   # 通过 Homebrew
   brew install sbt

**Linux**

.. code-block:: bash

   # 通过 SDKMAN
   curl -s "https://get.sdkman.io" | bash
   sdk install sbt

验证安装：

.. code-block:: bash

   sbt --version

IDE 选择
---------

.. list-table::
   :header-rows: 1

   * - IDE
     - 说明
   * - IntelliJ IDEA
     - 首选的 Scala IDE，安装 Scala 插件即可获得完整支持
   * - VS Code
     - 配合 Metals LSP 服务器，轻量选择
   * - Scala CLI
     - 交互式命令行，适合学习和快速实验

快速开始：Hello World
======================

创建 SBT 项目
--------------

.. code-block:: bash

   mkdir hello-scala
   cd hello-scala

创建 ``build.sbt``：

.. code-block:: scala
   :caption: build.sbt

   lazy val root = project
     .in(file("."))
     .settings(
       name         := "hello-scala",
       version      := "0.1.0",
       scalaVersion := "3.6.2",   // Scala 3 最新稳定版
     )

创建源码文件：

.. code-block:: scala
   :caption: src/main/scala/Main.scala

   @main def hello(): Unit =
     println("Hello, Scala!")

运行项目：

.. code-block:: bash

   sbt run
   # 输出: Hello, Scala!

.. note::

   使用 ``sbt new scala/scala3.g8`` 可快速生成标准 Scala 3 项目模板。

Scala 2 与 Scala 3
====================

+----------------+----------------------------+----------------------------+
| 特性           | Scala 2                    | Scala 3                    |
+================+============================+============================+
| 语法           | 传统 OOP/FP 混杂风格       | 更简洁，少用符号           |
+----------------+----------------------------+----------------------------+
| 隐式转换       | ``implicit`` 关键字        | ``given`` / ``using`` 子句 |
+----------------+----------------------------+----------------------------+
| 枚举           | ``sealed case class`` 模拟 | 原生 ``enum`` 关键字       |
+----------------+----------------------------+----------------------------+
| 联合类型       | 不支持                     | 原生 ``A | B`` 语法        |
+----------------+----------------------------+----------------------------+
| 宏             | 不稳定，API 复杂           | 稳定，基于引用的新系统     |
+----------------+----------------------------+----------------------------+
| 交集类型       | 不支持                     | 原生 ``A & B`` 语法        |
+----------------+----------------------------+----------------------------+

.. tip::

   新项目建议直接使用 **Scala 3**。本文档以 Scala 3 为主，必要时标注与 Scala 2 的差异。
