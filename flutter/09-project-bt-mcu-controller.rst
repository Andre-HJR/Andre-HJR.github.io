=====================================
开发项目：蓝牙单片机控制器
=====================================

项目简介
=========

**蓝牙单片机控制器（BLE MCU Controller）** 是一款 **Android 蓝牙（BLE）** 应用，
通过低功耗蓝牙 GATT 与单片机透传模块（HM-10 / JDY-08 / nRF52 / ESP32-BLE 等）通信，
实现「扫描 → 连接 → 发送指令 → 接收回传」的完整流程。

应用支持**文本 / HEX** 两种输入方式，并内置带 **CRC 校验的协议帧模式**：
发送时把指令打包成帧，接收时自动解包、丢弃坏帧。
其中 CRC 与帧打包由 **NDK 原生库（C 实现）** 提供，原生不可用时自动回退到纯 Dart 实现。

.. list-table::
   :header-rows: 1

   * - 项目属性
     - 说明
   * - 项目名称
     - bt_mcu_controller
   * - 项目定位
     - Android BLE 串口助手：与单片机透传模块通信
   * - 技术栈
     - Flutter 3.44.7 + Dart 3.12.2 + Kotlin + C（NDK）+ CMake
   * - 目标平台
     - Android 9.0（API 28）及以上，BLE 4.0+
   * - 许可证
     - GPL-3.0-or-later
   * - 代码规模
     - Dart 约 917 行（3 文件）+ 测试约 974 行 + C 原生 118 行

.. warning::

   本应用走 **BLE GATT 透传**。若你的模块是 HC-05 / HC-06 这类**经典蓝牙串口（SPP）**，
   BLE 方案不适用，需改用 ``flutter_bluetooth_serial`` 插件（仅 Android）。

技术栈与版本
==============

.. list-table::
   :header-rows: 1

   * - 项
     - 版本
     - 说明
   * - Flutter
     - 3.44.7 stable（Dart 3.12.2）
     -
   * - Android Gradle Plugin
     - 9.0.1
     - ``android/settings.gradle.kts``
   * - Gradle
     - 9.1.0
     - wrapper
   * - JDK
     - 17
     - Gradle 9 必须 JDK 17+
   * - compileSdk / minSdk / targetSdk
     - 36 / 28 / 36
     - minSdk 28 = Android 9
   * - NDK / CMake
     - 28.2.13676358 / 3.22.1
     - 编译 ``libmcu_native.so``
   * - flutter_blue_plus
     - 2.3.11
     - BLE 通信
   * - permission_handler
     - 11.4.0
     - 动态权限（锁定原因见下文）

系统架构
=========

应用采用 **Dart → MethodChannel → Kotlin/JNI → C（NDK）** 四层架构：

.. code-block::

   ┌─────────────────────────────────────────────────────┐
   │  Dart 层                                              │
   │  lib/main.dart         UI + 状态机（扫描/连接/收发/日志）│
   │  lib/mcu_frame.dart    ★ 协议帧「纯 Dart 参考实现」      │
   │  lib/mcu_native.dart    MethodChannel 桥接（带回退）    │
   └───────────────┬─────────────────────────────────────┘
                   │ MethodChannel "com.hjr.bt_mcu_controller/mcu_native"
                   │ （crc16 / encodeFrame 两个方法）
   ┌───────────────▼─────────────────────────────────────┐
   │  Kotlin 层                                            │
   │  MainActivity.kt     注册 MethodChannel，参数类型转换  │
   │  McuNative.kt        JNI 入口，System.loadLibrary      │
   └───────────────┬─────────────────────────────────────┘
                   │ JNI（符号 Java_com_hjr_bt_1mcu_1controller_*）
   ┌───────────────▼─────────────────────────────────────┐
   │  C / NDK 层                                           │
   │  mcu_native.c         CRC-16-CCITT + 帧编码（cmake）  │
   └─────────────────────────────────────────────────────┘

.. tip::

   架构的核心思想是 **「纯 Dart 规范 + 原生加速 + 自动降级」**：
   ``lib/mcu_frame.dart`` 是协议的规范实现，NDK C 实现与其**逐字节比对**，
   真机集成测试保证二者一致；原生不可用时静默回退到 Dart，功能不中断。

通信协议定义
==============

帧格式
---------

.. code-block::

   [0xAA] [0x55] [payload_len] [payload...] [crc_hi] [crc_lo] [0x0D] [0x0A]

.. list-table::
   :header-rows: 1

   * - 字段
     - 长度
     - 说明
   * - 帧头
     - 2
     - 固定 ``0xAA 0x55``
   * - 长度
     - 1
     - 负载字节数，>255 时截断为低 8 位（协议限制）
   * - 负载
     - ``len``
     - 实际指令数据
   * - CRC
     - 2
     - CRC-16-CCITT，高字节在前
   * - 帧尾
     - 2
     - 固定 ``0x0D 0x0A``，即 CR LF

帧相对负载的附加字节数 = 头2 + 长度1 + CRC2 + 尾2 = **7**，即 ``kFrameOverhead``。

CRC-16-CCITT
--------------

- 多项式 ``0x1021``，初值 ``0xFFFF``，按字节高位在前逐位运算，**无结果异或**；
- 标准测试向量：ASCII ``"123456789"`` → ``0x29B1``；两个实现都必须通过。

发送样例
---------

文本 ``A`` 对应字节 ``0x41``，打包后：

.. code-block::

   AA 55 01 41 B9 15 0D 0A

核心模块详解
==============

``lib/mcu_frame.dart`` — 协议帧纯 Dart 参考实现
-------------------------------------------------

.. list-table::
   :header-rows: 1

   * - API
     - 作用
   * - ``crc16Ccitt(List<int>)``
     - CRC-16-CCITT
   * - ``encodeFrame(List<int>)``
     - 负载 → 完整帧
   * - ``tryDecodeFrame(List<int>)``
     - 从帧头开始解析一帧，返回 ``(payload, total)`` 或 ``null``
   * - ``hasFrameHeader(List<int>)``
     - 是否以 ``0xAA 0x55`` 开头
   * - ``FrameReceiver``
     - 流式拼帧器，处理四种情况

``FrameReceiver.add(chunk)`` 处理四种情况：

1. **拆包**：一帧分多段到达 → 缓冲累积，凑齐后再出帧；
2. **粘包**：一段含多帧 → 顺序出多帧；
3. **坏帧**：CRC/帧尾不符 → 丢 1 字节重同步（不会永久卡死）；
4. **杂字节**：帧头前的垃圾 → 跳过；末尾孤立 ``0xAA`` → 保留等下一段补全帧头。

``lib/mcu_native.dart`` — MethodChannel 桥接
------------------------------------------------

- channel 名：``com.hjr.bt_mcu_controller/mcu_native``；
- ``crc16`` / ``encodeFrame``：调用原生，抛异常或返回 ``null`` 时自动回退到 Dart；
- ``isNativeAvailable()``：空负载探测原生是否真的加载。

.. important::

   **字节契约**：负载必须用 ``Uint8List`` 发送。标准编解码器把 ``Uint8List`` 编码为
   ``byte[]``，即 Kotlin 侧 ``call.argument<ByteArray>``，而普通 ``List<int>`` 会编码为
   ``List<Integer>``，原生侧强转 ``ByteArray`` 会抛 ``ClassCastException``，
   导致静默回退到 Dart —— 这是本项目最重要的一个坑，详见「开发记录」。

NDK 原生层（C 实现）
---------------------

- ``android/app/src/main/cpp/mcu_native.c``：``crc16_ccitt()`` + ``encode_frame()``；
- JNI 导出符号对包名中的 ``_`` 做了 ``_1`` 转义：
  ``Java_com_hjr_bt_1mcu_1controller_McuNative_{crc16,encodeFrame}``；
- ``McuNative.kt``：``object``，``init { System.loadLibrary("mcu_native") }``。

``lib/main.dart`` — 界面与状态机
-----------------------------------

- **状态**：``_scanning`` / ``_connected`` / ``_selectedId`` / ``_hexMode`` / ``_frameMode``，
  纯 ``setState`` 手写状态机；
- **权限**：``bluetoothScan`` + ``bluetoothConnect`` + ``location``，因 Android 6~11 扫描 BLE 需要定位权限；
- **连接**：``connect(license: License.nonprofit)`` → ``discoverServices()`` →
  按 UUID 找服务/特征 → 使能 notify 并监听 ``onValueReceived``；
- **发送**：协议帧模式先经原生打包，``withoutResponse: true`` 写入；
- **日志**：每条含时间戳、方向、ASCII 与 HEX 双视图，最多 500 条，自动滚底；
- **设置**：右上角 ⚙ 可改服务/特征 UUID（默认 FFE0 / FFE1）。

测试策略
=========

.. code-block::

   test/
   ├── mcu_frame_test.dart            # 协议帧/CRC 的「规范」测试（24 个）
   ├── mcu_native_bridge_test.dart    # MethodChannel 桥接契约 + 回退（9 个）
   ├── widget_test.dart               # parseHex 边界（6 个）
   ├── ui_test.dart                   # 界面 widget 测试（6 个，mock BLE 平台）
   └── mock_blue_plus.dart            # 测试替身库（FakeBluePlusPlatform）
   integration_test/
   └── mcu_native_integration_test.dart  # 真机比对：C 实现 vs Dart 参考

- **单元测试**：CRC 标准向量、帧编解码、``FrameReceiver`` 拆包/粘包/坏帧重同步/杂字节；
- **桥接契约测试**：验证 Dart→原生的方法名、参数 key、**字节类型必须是 ``Uint8List``**，
  以及原生抛异常/返回 null/无 handler 三种情况下的 Dart 回退；
- **界面 widget 测试**：通过替换 ``FlutterBluePlusPlatform.instance`` 为
  ``FakeBluePlusPlatform``，纯测试环境走通「扫描→连接→文本发送→接收→断开」全流程；
- **真机集成测试**：断言 ``isNativeAvailable() == true``，对 8 组负载
  （空、单字节、边界值、标准向量、满 255、超 255、帧定界符、中文 UTF-8）
  比对 C 与 Dart 输出**逐字节一致**。

开发记录（关键踩坑）
======================

字节契约 Bug（Dart → Kotlin）
-------------------------------

**现象**：真机上 ``encodeFrame`` 静默回退到 Dart，原生路径从未生效。

**根因**：Dart 侧发送的是普通 ``List<int>``，标准编解码器编码成 ``List<Integer>``，
Kotlin ``call.argument<ByteArray>`` 强转抛 ``ClassCastException``，被桥接的 try/catch 吞掉后回退。

**修复**：一律发送 ``Uint8List.fromList(...)``；测试里用 ``isA<Uint8List>()`` 锁定该契约。

``FRAME_OVERHEAD`` 越界 Bug（C）
---------------------------------

**现象**：原生返回的帧缺最后一个 ``0x0A``，且写帧时越界 1 字节（堆溢出）。

**根因**：``mcu_native.c`` 里 ``FRAME_OVERHEAD`` 定义为 6，但完整帧是 ``len + 7`` 字节
（注释自己算出 2+1+2+2=7 却写成 6），写入索引 ``len+6`` 造成**堆越界写 1 字节**。

**修复**：改为 7，与 Dart 侧 ``kFrameOverhead = 7`` 对齐。
此 bug 编译期不报错、单测 mock 不到，正是真机集成测试要抓的那类问题。

MethodChannel ``<Uint8List>`` 强转
------------------------------------

``invokeMethod<Uint8List>`` 内部做 ``decodeEnvelope as Uint8List?``，mock 若返回普通
``List<int>`` 会抛 ``_TypeError``；**mock 必须返回 ``Uint8List.fromList(...)``**。

UI 测试的坑
-------------

- ``DropdownButtonFormField`` 关闭时不渲染设备项：测试以
  ``FlutterBluePlus.lastScanResults`` 作为「设备已出现」的同步信号；
- 协议帧发送测试需切回**文本模式**：HEX 模式下输入 ``A`` 会被判为奇数长度非法输入，
  ``encodeFrame`` 从未被调用；
- ``FlutterBluePlus`` 静态状态跨测试残留：用共享 fake + 独立 remoteId + 扫描重置。

版本锁定
---------

- ``permission_handler`` 锁顶层 11.x（``^11.3.1`` → 11.4.0；Android 插件
  ``permission_handler_android`` 实际解析为 12.1.0）：pubspec 注释认为 12.x 及以上
  Android 插件要求 ``compileSdk >= 37``，与当前 Flutter/AGP 9.0.1（默认 compileSdk 36）
  不兼容，故锁 11.x；
- ``flutter_blue_plus`` 的 ``connect()`` 必须传 ``license``，
  个人/教育用 ``License.nonprofit`` 免费。

构建与使用
============

.. code-block:: bash

   flutter pub get
   flutter analyze                  # 静态检查，要求 0 问题
   flutter test                     # 单元 + widget 测试，要求全绿
   flutter build apk --debug        # 验证 NDK cmake 编译并打包
   flutter test integration_test/   # 真机比对（需设备）

产物：``build/app/outputs/flutter-apk/app-debug.apk``，内含
``lib/arm64-v8a|armeabi-v7a|x86_64/libmcu_native.so``。

使用方法
---------

1. 安装 APK，首次启动授权蓝牙 / 定位权限；
2. 点击「扫描」，选择单片机对应的设备，点击「连接」；
3. 输入指令：
   - **文本模式**：直接输入 ASCII 指令（如 ``A``、``B``、``Q``）；
   - **HEX 模式**：输入十六进制字节，如 ``A1 B2 0C``，允许空格/逗号分隔；
4. 单片机回传的数据实时显示在日志区（ASCII 与 HEX 双视图）。

自定义服务 / 特征 UUID
------------------------

默认值（HM-10 透传常用）：服务 ``0000ffe0-...``、写入/通知特征 ``0000ffe1-...``。
点击右上角 **⚙ 设置** 修改，修改后需重新连接生效。

已知限制
=========

- **经典蓝牙 SPP 不支持**：HC-05 / HC-06 需改用 ``flutter_bluetooth_serial``；
- **集成测试需真机**：``flutter test integration_test/`` 依赖设备；
- **帧长限制**：负载 >255 时长度字段截断，业务侧应控制单帧 ≤255 字节。

相关链接
=========

- 仓库：``D:\HJR\FTools\bt_mcu_controller``
- 开发文档：``docs/开发文档.md``，含架构、协议、测试策略与踩坑记录
- 应用说明：``README.md``
