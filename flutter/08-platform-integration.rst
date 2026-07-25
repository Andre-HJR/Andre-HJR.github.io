======================
平台集成与发布
======================

Flutter 通过平台通道与原生代码交互，并提供完善的打包发布能力。

平台通道（Platform Channel）
=============================

Flutter 与原生代码之间通过 **MethodChannel** 通信：

.. code-block::

   Flutter (Dart)          Native (Kotlin/Swift)
   ┌────────────┐         ┌──────────────────┐
   │ 调用方法    │ ──────→ │ 处理并返回结果    │
   │ await       │ ←────── │                   │
   │ result      │         │                   │
   └────────────┘         └──────────────────┘
          MethodChannel

Dart 端（Flutter）
-------------------

.. code-block:: dart

   import 'package:flutter/services.dart';

   class BatteryPlugin {
     static const _channel = MethodChannel('com.example/battery');

     static Future<int> getBatteryLevel() async {
       try {
         final level = await _channel.invokeMethod('getBatteryLevel');
         return level as int;
       } on PlatformException catch (e) {
         print('获取电量失败: ${e.message}');
         return -1;
       }
     }
   }

Android 端（Kotlin）
---------------------

.. code-block:: kotlin

   // MainActivity.kt
   class MainActivity : FlutterActivity() {
     private val CHANNEL = "com.example/battery"

     override fun configureFlutterEngine(flutterEngine: FlutterEngine) {
       super.configureFlutterEngine(flutterEngine)
       MethodChannel(flutterEngine.dartExecutor.binaryMessenger, CHANNEL)
         .setMethodCallHandler { call, result ->
           if (call.method == "getBatteryLevel") {
             val batteryLevel = getBatteryLevel()
             if (batteryLevel != -1) {
               result.success(batteryLevel)
             } else {
               result.error("UNAVAILABLE", "无法获取电量", null)
             }
           } else {
             result.notImplemented()
           }
         }
     }

     private fun getBatteryLevel(): Int {
       val intent = registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
       val level = intent?.getIntExtra(BatteryManager.EXTRA_LEVEL, -1) ?: -1
       val scale = intent?.getIntExtra(BatteryManager.EXTRA_SCALE, -1) ?: -1
       return if (level > 0 && scale > 0) (level * 100 / scale) else -1
     }
   }

iOS 端（Swift）
---------------

.. code-block:: swift

   // AppDelegate.swift
   import Flutter
   import UIKit

   @main
   @objc class AppDelegate: FlutterAppDelegate {
     override func application(
       _ application: UIApplication,
       didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?
     ) -> Bool {
       let controller = window?.rootViewController as! FlutterViewController
       let channel = FlutterMethodChannel(
         name: "com.example/battery",
         binaryMessenger: controller.binaryMessenger
       )
       channel.setMethodCallHandler { (call, result) in
         if call.method == "getBatteryLevel" {
           let device = UIDevice.current
           device.isBatteryMonitoringEnabled = true
           let level = Int(device.batteryLevel * 100)
           if level >= 0 {
             result(level)
           } else {
             result(FlutterError(code: "UNAVAILABLE", message: "无法获取电量", details: nil))
           }
         } else {
           result(FlutterMethodNotImplemented)
         }
       }
       GeneratedPluginRegistrant.register(with: self)
       return super.application(application, didFinishLaunchingWithOptions: launchOptions)
     }
   }

.. tip::

   使用 **Pigeon** 可以自动生成类型安全的平台通道代码，避免手动编写样板代码。

常用插件（packages）
=====================

.. list-table::
   :header-rows: 1

   * - 插件
     - 用途
     - 状态
   * - url_launcher
     - 打开 URL、拨打电话、发送邮件
     - 官方
   * - image_picker
     - 从相册/相机获取图片/视频
     - 官方
   * - permission_handler
     - 权限管理（相机、位置、通知等）
     - 社区
   * - local_auth
     - 生物识别认证（指纹、Face ID）
     - 官方
   * - camera
     - 相机预览与拍照
     - 官方
   * - geolocator
     - GPS 定位
     - 社区
   * - flutter_local_notifications
     - 本地通知
     - 社区
   * - firebase_core
     - Firebase 核心（需配合其他 Firebase 插件）
     - 官方
   * - share_plus
     - 系统分享面板
     - 社区
   * - file_picker
     - 文件选择器
     - 社区

.. note::

   使用插件前请查阅其 pub.dev 页面的 ``Platforms`` 标签，
   确认它支持你的目标平台。

应用打包与发布
===============

Android 发布
-------------

.. code-block:: bash

   # 1. 生成签名密钥
   keytool -genkey -v -keystore release.jks \
     -keyalg RSA -keysize 2048 -validity 10000 \
     -alias release

   # 2. 创建 key.properties
   # 在 android/ 目录下创建 key.properties：
   #   storePassword=xxx
   #   keyPassword=xxx
   #   keyAlias=release
   #   storeFile=release.jks

   # 3. 构建 APK / AAB
   flutter build apk                # APK
   flutter build appbundle          # AAB（推荐上传 Google Play）

   # 产物路径：
   # build/app/outputs/flutter-apk/app-release.apk
   # build/app/outputs/bundle/release/app-release.aab

iOS 发布
--------

.. code-block:: bash

   # 1. 在 Xcode 中配置证书和签名
   open ios/Runner.xcworkspace

   # 2. 构建 IPA
   flutter build ios

   # 3. 通过 Xcode Archive 发布
   # Product → Archive → Distribute App

Web 发布
--------

.. code-block:: bash

   flutter build web
   # 产物在 build/web/ 目录

   # 部署到任意静态文件服务器
   # 或 Firebase Hosting
   firebase deploy --only hosting

Windows / macOS / Linux 发布
------------------------------

.. code-block:: bash

   flutter build windows    # Windows 安装包
   flutter build macos      # macOS .app（需 Mac）
   flutter build linux      # Linux 应用

.. important::

   发布前务必：
   - 更新版本号（``pubspec.yaml`` 中的 ``version`` 字段）
   - 检查应用图标和启动页
   - 清理调试日志和打印语句
   - 测试不同设备和屏幕尺寸

CI/CD 与测试
=============

测试类型
---------

.. code-block:: dart

   // 单元测试
   void main() {
     test('Counter 初始值应为 0', () {
       final counter = Counter();
       expect(counter.value, 0);
     });
   }

   // Widget 测试
   testWidgets('Counter 按钮应增加计数值', (tester) async {
     await tester.pumpWidget(const MyApp());
     await tester.tap(find.byIcon(Icons.add));
     await tester.pump();
     expect(find.text('1'), findsOneWidget);
   });

CI/CD 配置
-----------

.. code-block:: yaml
   :caption: .github/workflows/flutter.yml

   name: Flutter CI
   on: [push, pull_request]

   jobs:
     build:
       runs-on: ubuntu-latest
       steps:
         - uses: actions/checkout@v4
         - uses: subosito/flutter-action@v2
           with:
             flutter-version: '3.x'
         - run: flutter pub get
         - run: flutter analyze
         - run: flutter test
         - run: flutter build apk

贴士与最佳实践
===============

- **优先使用 Flutter 内置 API**，仅在必要时编写平台通道代码
- **使用 Pigeon** 自动生成安全的平台通道接口
- **插件筛选**：优先选择官方插件（flutter.dev 维护），确认 pub.dev 评分和维护状态
- **及时更新**：保持 Flutter SDK 和依赖的最新稳定版本
- **测试覆盖**：至少为关键业务逻辑编写单元测试
- **性能分析**：使用 ``flutter devtools`` 进行性能分析和内存泄漏排查
