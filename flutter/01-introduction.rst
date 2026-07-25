==========================
Flutter 简介与环境搭建
==========================

Flutter 框架概述
=================

Flutter 是 Google 开源的 **跨平台 UI 框架**，使用 **Dart** 语言开发，通过自绘引擎
Skia（或 Impeller）实现高性能渲染。

核心特点
---------

- **一套代码，多端运行**：iOS、Android、Web、Windows、macOS、Linux
- **自绘引擎**：不依赖平台原生控件，保证跨平台一致性
- **Hot Reload**：亚秒级热重载，开发体验极佳
- **声明式 UI**：通过 Widget 组合构建界面
- **高性能**：直接编译为原生代码（ARM / x64 / WebAssembly）

架构层次
---------

.. code-block::

   ┌─────────────────────────────────┐
   │          Flutter App            │
   │     (你的 Dart 代码 + Widget)   │
   ├─────────────────────────────────┤
   │       Framework (Dart)          │
   │  Material / Cupertino / Widgets │
   │  Rendering / Animation / Gesture│
   ├─────────────────────────────────┤
   │          Engine (C++)           │
   │   Skia/Impeller · Dart VM · Text│
   ├─────────────────────────────────┤
   │       Platform (Shell)          │
   │  Android (Kotlin/Java) · iOS    │
   │  (Obj-C/Swift) · Web · Desktop  │
   └─────────────────────────────────┘

环境搭建
=========

安装 Flutter SDK
-----------------

**Windows**

.. code-block:: bash

   # 下载 Flutter SDK 压缩包
   # https://docs.flutter.dev/get-started/install/windows

   # 解压到目标目录，例如 D:\ProgramData\flutter
   # 将 flutter\bin 添加到 PATH 环境变量

   flutter --version

**macOS**

.. code-block:: bash

   # 通过 Homebrew
   brew install --cask flutter

   # 或手动下载
   # https://docs.flutter.dev/get-started/install/macos

**Linux**

.. code-block:: bash

   # 通过 snap
   sudo snap install flutter --classic

   # 或手动下载
   # https://docs.flutter.dev/get-started/install/linux

.. tip::

   推荐使用 **FVM（Flutter Version Management）** 管理多个 Flutter 版本：

   .. code-block:: bash

      dart pub global activate fvm
      fvm install stable
      fvm use stable

安装 IDE 插件
--------------

.. list-table::
   :header-rows: 1

   * - IDE
     - 插件
   * - Android Studio
     - 内置 Flutter 和 Dart 插件
   * - VS Code
     - 安装 **Flutter** 和 **Dart** 扩展
   * - IntelliJ IDEA
     - 安装 Flutter 和 Dart 插件

验证安装
---------

.. code-block:: bash

   flutter doctor

``flutter doctor`` 会检查所有依赖并给出报告：

.. code-block:: text

   Doctor summary (to see all details, run flutter doctor -v):
   [✓] Flutter (Channel stable, 3.x.x)
   [✓] Windows Version
   [✓] Android toolchain
   [✓] Chrome - develop for the web
   [✓] Visual Studio - develop for Windows
   [!] Android Studio (not configured)
   [✓] VS Code (with Flutter extensions)
   [✓] Connected devices

.. note::

   出现 ``[!]`` 表示可选项，不影响开发。``[✗]`` 表示缺少必要组件，需解决。

快速开始：第一个 Flutter 应用
===============================

.. code-block:: bash

   flutter create my_app
   cd my_app
   flutter run

生成的默认应用：

.. code-block:: dart
   :caption: lib/main.dart

   import 'package:flutter/material.dart';

   void main() {
     runApp(const MyApp());
   }

   class MyApp extends StatelessWidget {
     const MyApp({super.key});

     @override
     Widget build(BuildContext context) {
       return MaterialApp(
         title: 'Flutter Demo',
         theme: ThemeData(
           colorSchemeSeed: Colors.blue,
           useMaterial3: true,
         ),
         home: const MyHomePage(title: 'Flutter Demo'),
       );
     }
   }

   class MyHomePage extends StatefulWidget {
     const MyHomePage({super.key, required this.title});
     final String title;

     @override
     State<MyHomePage> createState() => _MyHomePageState();
   }

   class _MyHomePageState extends State<MyHomePage> {
     int _counter = 0;

     void _incrementCounter() {
       setState(() => _counter++);
     }

     @override
     Widget build(BuildContext context) {
       return Scaffold(
         appBar: AppBar(title: Text(widget.title)),
         body: Center(
           child: Column(
             mainAxisAlignment: MainAxisAlignment.center,
             children: [
               const Text('You have pushed the button this many times:'),
               Text(
                 '$_counter',
                 style: Theme.of(context).textTheme.headlineMedium,
               ),
             ],
           ),
         ),
         floatingActionButton: FloatingActionButton(
           onPressed: _incrementCounter,
           child: const Icon(Icons.add),
         ),
       );
     }
   }

开发流程
=========

.. grid:: 2

   .. grid-item-card:: 🚀 调试模式
      ^^^^^^^^^^^^^^^^
      - ``flutter run``：在连接的设备上运行
      - ``flutter run -d chrome``：在 Chrome 中运行
      - ``flutter run -d windows``：在 Windows 桌面运行
      - Hot Reload（``r`` 键） / Hot Restart（``R`` 键）

   .. grid-item-card:: 📦 发布模式
      ^^^^^^^^^^^^^^^^
      - ``flutter build apk``：Android APK
      - ``flutter build ios``：iOS IPA（需 macOS）
      - ``flutter build web``：Web 部署
      - ``flutter build windows``：Windows 安装包

.. important::

   **Hot Reload** 可以保留应用状态注入代码变更，是最常用的开发方式。
   如果更改了原生代码或全局变量，使用 **Hot Restart**。
