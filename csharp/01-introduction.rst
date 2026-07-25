======================================
C# 与 .NET 开发环境
======================================

C# 语言概述
=============

C# 是微软推出的一种 **类型安全**、 **面向对象** 的编程语言，由 Anders Hejlsberg 领导设计，
2002 年随 .NET Framework 首次发布。

核心特点
---------

- **托管执行**：运行在 CLR (Common Language Runtime) 上，自动 GC
- **类型安全**：强类型语言，编译时捕获类型错误
- **面向对象**：类、继承、接口、委托
- **函数式特性**：LINQ、Lambda、模式匹配
- **异步编程**：async/await 原生支持
- **跨平台**：.NET Core / .NET 5+ 支持 Linux/macOS

.NET 平台演进
==============

.. list-table::
   :header-rows: 1

   * - 版本
     - 发布时间
     - 特性
     - 状态
   * - .NET Framework 4.0
     - 2010
     - WPF 4.0, MEF
     - 仅 Windows
   * - .NET Framework 4.8
     - 2019
     - 最终版本
     - 仅 Windows
   * - .NET Core 3.1
     - 2019
     - WPF/WinForms 支持
     - LTS（已结束）
   * - .NET 6
     - 2021
     - 统一平台
     - LTS
   * - .NET 8
     - 2023
     - AOT 编译
     - LTS（当前）
   * - .NET 9
     - 2024
     - 最新版本
     - STS

.. tip::

   对于新项目，推荐使用 **.NET 8** 或 **.NET 9** (跨平台)。
   对于 Windows 桌面应用（WinForms / WPF），.NET 6+ 提供了完整支持。

开发环境搭建
=============

安装 Visual Studio 2022
------------------------

.. code-block:: text

   1. 下载 Visual Studio 2022 Community（免费）
      https://visualstudio.microsoft.com/

   2. 安装时选择工作负载（Workloads）：
      - .NET 桌面开发（WinForms / WPF）
      - ASP.NET 和 Web 开发
      - 数据存储和处理

   3. 可选：安装 ReSharper（JetBrains 代码分析工具）

创建第一个项目
----------------

.. code-block:: text

   1. 文件 → 新建 → 项目
   2. 搜索 "Console App" → 选择 "控制台应用 (.NET 8)"
   3. 项目名称: HelloCSharp
   4. 点击 "创建"

.. code-block:: csharp
   :caption: Program.cs

   // 顶级语句（Top-level Statements，C# 9+）
   Console.WriteLine("Hello, C#!");

   // 传统写法（C# 8 及以前）
   namespace HelloCSharp
   {
       class Program
       {
           static void Main(string[] args)
           {
               Console.WriteLine("Hello, C#!");
           }
       }
   }

项目结构
=========

.. code-block::

   HelloCSharp/
   ├── HelloCSharp.sln           # 解决方案文件
   ├── HelloCSharp.csproj        # 项目文件（SDK 风格）
   ├── Program.cs                # 入口文件
   ├── Models/                   # 数据模型
   ├── Services/                 # 业务逻辑
   └── Properties/
       └── launchSettings.json

SDK 风格的项目文件：

.. code-block:: xml
   :caption: HelloCSharp.csproj

   <Project Sdk="Microsoft.NET.Sdk">
     <PropertyGroup>
       <OutputType>Exe</OutputType>
       <TargetFramework>net8.0</TargetFramework>
       <Nullable>enable</Nullable>
       <ImplicitUsings>enable</ImplicitUsings>
     </PropertyGroup>
   </Project>

.. note::

   ``ImplicitUsings`` 自动引入常用命名空间：
   ``System``、``System.Collections.Generic``、``System.Linq``、
   ``System.Threading.Tasks`` 等，无需手动 ``using``。

C# 与 VB.NET / F# 对比
=========================

.. list-table::
   :header-rows: 1

   * - 特性
     - C#
     - VB.NET
     - F#
   * - 风格
     - 类 C 语法
     - 类 BASIC 语法
     - 函数式
   * - 类型系统
     - 强类型
     - 强类型
     - 强类型 + 类型推断
   * - 不可变性
     - 手动
     - 手动
     - 默认不可变
   * - 异步
     - async/await
     - Async/Await
     - async/await + 计算表达式
   * - 适用场景
     - 通用
     - 遗留系统维护
     - 数据处理/科学计算

.. tip::

   **学习路线建议：**

   1. C# 语言基础（类型、控制流、OOP）
   2. .NET 框架（BCL、集合、文件 I/O）
   3. WinForms（入门简单，快速做工具）
   4. WPF（MVVM 模式，现代桌面应用）
   5. ASP.NET Core（Web 后端）
