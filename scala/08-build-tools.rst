==================
构建工具与生态
==================

SBT（Scala Build Tool）
========================

SBT 是 Scala 生态的标配构建工具，支持增量编译、依赖管理和任务自动化。

build.sbt 配置
--------------

.. code-block:: scala
   :caption: build.sbt

   lazy val root = project
     .in(file("."))
     .settings(
       name         := "my-project",
       version      := "0.1.0",
       scalaVersion := "3.6.2",

       // 依赖管理
       libraryDependencies ++= Seq(
         "org.typelevel" %% "cats-effect" % "3.5.7",
         "org.scalatest" %% "scalatest"   % "3.2.19" % Test,
       ),

       // 编译器选项
       scalacOptions ++= Seq(
         "-deprecation",           // 弃用警告
         "-feature",               // 特性警告
         "-unchecked",             // 未检查警告
         "-Xfatal-warnings",       // 警告视为错误
       ),
     )

依赖配置语法
------------

.. list-table::
   :header-rows: 1

   * - 写法
     - 说明
   * - ``"org" %% "name" % "ver"``
     - 自动添加当前 Scala 版本后缀
   * - ``"org" % "name" % "ver"``
     - Java 库（无版本后缀）
   * - ``% Test``
     - 仅测试范围（等价于 Maven test scope）
   * - ``% Provided``
     - 编译时提供，打包时排除

常用命令
---------

.. code-block:: bash

   sbt compile          # 编译
   sbt test             # 运行测试
   sbt run              # 运行主类
   sbt console          # Scala REPL（含项目依赖）
   sbt clean            # 清理构建产物
   sbt reload            # 重新加载 build.sbt 变更
   sbt compile --watch   # 持续编译（文件变更自动重编译）

多模块项目
----------

.. code-block:: scala
   :caption: build.sbt

   lazy val core = project
     .in(file("modules/core"))
     .settings(
       name := "my-core",
       libraryDependencies += "org.typelevel" %% "cats-core" % "2.13.0",
     )

   lazy val app = project
     .in(file("modules/app"))
     .dependsOn(core)    // 依赖 core 模块
     .settings(
       name := "my-app",
     )

测试框架
=========

ScalaTest
----------

.. code-block:: scala

   import org.scalatest.flatspec.AnyFlatSpec
   import org.scalatest.matchers.should.Matchers

   class CalculatorSpec extends AnyFlatSpec with Matchers:
     "A calculator" should "add two numbers" in:
       val result = 2 + 3
       result shouldEqual 5

     it should "multiply two numbers" in:
       (3 * 4) should be > 10

.. tip::

   ScalaTest 支持多种风格（FlatSpec、WordSpec、FunSuite 等）。
   ``AnyFlatSpec`` 是最常用的一种，可读性好。

MUnit
------

轻量级测试框架，由 Scala Center 维护：

.. code-block:: scala

   import munit.FunSuite

   class MySuite extends FunSuite:
     test("addition") {
       assertEquals(2 + 3, 5)
     }

     test("exception") {
       intercept[ArithmeticException]:
         1 / 0
     }

常用框架与库
=============

.. list-table::
   :header-rows: 1

   * - 框架
     - 用途
     - 官网
   * - **Apache Spark**
     - 大数据分布式计算引擎
     - https://spark.apache.org/
   * - **Akka**
     - Actor 模型并发与分布式系统
     - https://akka.io/
   * - **Play**
     - 全栈 Web 框架（类型安全路由、热重载）
     - https://www.playframework.com/
   * - **http4s**
     - 纯函数式 HTTP 服务
     - https://http4s.org/
   * - **cats**
     - 纯函数式编程核心库
     - https://typelevel.org/cats/
   * - **ZIO**
     - 零依赖的函数式并发与 I/O 库
     - https://zio.dev/
   * - **circe**
     - JSON 编解码（函数式）
     - https://circe.github.io/circe/
   * - **Doobie**
     - 纯函数式 JDBC 数据库访问层
     - https://tpolecat.github.io/doobie/
   * - **Tapir**
     - 类型安全的 HTTP API 定义
     - https://tapir.softwaremill.com/
   * - **ScalaCheck**
     - 属性基测试（自动生成测试用例）
     - https://scalacheck.org/

学习资源
=========

.. grid:: 2

   .. grid-item-card:: 官方资源
      ^^^^^^^^^^^^^^^^
      - `Scala 官方文档 <https://docs.scala-lang.org/>`__
      - `Scala 3 书籍 <https://docs.scala-lang.org/scala3/book/>`__
      - `Scala 标准库 API <https://scala-lang.org/api/3.x/>`__

   .. grid-item-card:: 推荐书籍
      ^^^^^^^^^^^^^^^^
      - **Programming in Scala** (Martin Odersky 等)
      - **Functional Programming in Scala** (Paul Chiusano 等)
      - **Scala with Cats** (Noel Welsh 等)

   .. grid-item-card:: 在线课程
      ^^^^^^^^^^^^^^^^
      - Coursera: Functional Programming in Scala
      - Rock the JVM（视频教程）
      - Scala Bridge（入门工作坊）

   .. grid-item-card:: 社区与资讯
      ^^^^^^^^^^^^^^^^
      - `Scala Center <https://scala.epfl.ch/>`__
      - `Typelevel <https://typelevel.org/>`__
      - `Reddit r/scala <https://reddit.com/r/scala>`__
