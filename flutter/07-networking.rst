======================
网络与数据持久化
======================

现代应用几乎都需要网络通信和数据存储能力。

HTTP 网络请求
===============

添加依赖
---------

.. code-block:: yaml
   :caption: pubspec.yaml

   dependencies:
     http: ^1.0.0          # 基础 HTTP 客户端
     dio: ^5.0.0           # 功能更丰富的 HTTP 客户端（可选）

基本 GET 请求
--------------

.. code-block:: dart

   import 'dart:convert';
   import 'package:http/http.dart' as http;

   Future<Map<String, dynamic>> fetchUser(int id) async {
     final response = await http.get(
       Uri.parse('https://api.example.com/users/$id'),
       headers: {'Accept': 'application/json'},
     );

     if (response.statusCode == 200) {
       return json.decode(response.body) as Map<String, dynamic>;
     } else {
       throw Exception('请求失败: ${response.statusCode}');
     }
   }

POST 请求
----------

.. code-block:: dart

   Future<Map<String, dynamic>> createUser(String name, String email) async {
     final response = await http.post(
       Uri.parse('https://api.example.com/users'),
       headers: {'Content-Type': 'application/json'},
       body: json.encode({
         'name': name,
         'email': email,
       }),
     );

     if (response.statusCode == 201) {
       return json.decode(response.body) as Map<String, dynamic>;
     } else {
       throw Exception('创建失败: ${response.statusCode}');
     }
   }

Dio（功能更完善的 HTTP 库）
============================

.. code-block:: dart

   import 'package:dio/dio.dart';

   final dio = Dio(BaseOptions(
     baseUrl: 'https://api.example.com',
     connectTimeout: const Duration(seconds: 5),
     receiveTimeout: const Duration(seconds: 3),
     headers: {
       'Content-Type': 'application/json',
     },
   ));

   // 拦截器（统一处理 Token、日志、错误）
   dio.interceptors.add(InterceptorsWrapper(
     onRequest: (options, handler) {
       options.headers['Authorization'] = 'Bearer $token';
       handler.next(options);
     },
     onResponse: (response, handler) {
       handler.next(response);
     },
     onError: (error, handler) {
       handler.next(error);
     },
   ));

   // 使用 Dio 请求
   Future<User> getUser(int id) async {
     final response = await dio.get('/users/$id');
     return User.fromJson(response.data);
   }

.. tip::

   Dio 相比 ``http`` 的优势：
   - 拦截器（统一处理认证、日志、重试）
   - 请求取消
   - 自动序列化
   - 文件上传/下载进度
   - 连接超时和重试机制

JSON 序列化
============

手动序列化
-----------

.. code-block:: dart

   class User {
     final int id;
     final String name;
     final String? email;

     const User({
       required this.id,
       required this.name,
       this.email,
     });

     // JSON → Model
     factory User.fromJson(Map<String, dynamic> json) {
       return User(
         id: json['id'] as int,
         name: json['name'] as String,
         email: json['email'] as String?,
       );
     }

     // Model → JSON
     Map<String, dynamic> toJson() {
       return {
         'id': id,
         'name': name,
         'email': email,
       };
     }
   }

使用 json_serializable（推荐）
-------------------------------

添加依赖：

.. code-block:: yaml
   :caption: pubspec.yaml

   dependencies:
     json_annotation: ^4.0.0

   dev_dependencies:
     json_serializable: ^6.0.0
     build_runner: ^2.0.0

定义模型：

.. code-block:: dart

   import 'package:json_annotation/json_annotation.dart';

   part 'user.g.dart';    // 自动生成的文件

   @JsonSerializable()
   class User {
     final int id;
     final String name;

     @JsonKey(name: 'email_address')   // 映射不同名的字段
     final String? email;

     final DateTime? createdAt;

     const User({
       required this.id,
       required this.name,
       this.email,
       this.createdAt,
     });

     factory User.fromJson(Map<String, dynamic> json) =>
         _$UserFromJson(json);

     Map<String, dynamic> toJson() => _$UserToJson(this);
   }

运行代码生成：

.. code-block:: bash

   dart run build_runner build    # 一次性生成
   dart run build_runner watch   # 持续监听

.. tip::

   使用 ``@JsonKey()`` 注解处理字段名映射、默认值和忽略字段：
   ``@JsonKey(name: 'full_name', defaultValue: 'anonymous')``

本地数据持久化
===============

SharedPreferences（键值存储）
-------------------------------

适合存储小型配置数据：

.. code-block:: yaml
   :caption: pubspec.yaml

   dependencies:
     shared_preferences: ^2.0.0

.. code-block:: dart

   // 写入
   final prefs = await SharedPreferences.getInstance();
   await prefs.setString('user_token', 'abc123');
   await prefs.setInt('counter', 42);
   await prefs.setBool('is_dark_mode', true);

   // 读取
   final token = prefs.getString('user_token');
   final counter = prefs.getInt('counter') ?? 0;

   // 删除
   await prefs.remove('user_token');
   await prefs.clear();

文件存储（path_provider）
--------------------------

适合存储文件、日志、图片：

.. code-block:: dart

   import 'dart:io';
   import 'package:path_provider/path_provider.dart';

   // 获取应用文档目录
   Future<Directory> getDocDir() async {
     return await getApplicationDocumentsDirectory();
   }

   // 写入文件
   Future<void> writeFile(String name, String content) async {
     final dir = await getDocDir();
     final file = File('${dir.path}/$name');
     await file.writeAsString(content);
   }

   // 读取文件
   Future<String> readFile(String name) async {
     final dir = await getDocDir();
     final file = File('${dir.path}/$name');
     return await file.readAsString();
   }

SQLite 数据库（sqflite）
-------------------------

适合存储结构化数据：

.. code-block:: yaml
   :caption: pubspec.yaml

   dependencies:
     sqflite: ^2.0.0
     path: ^1.0.0

.. code-block:: dart

   import 'package:sqflite/sqflite.dart';
   import 'package:path/path.dart';

   class DatabaseHelper {
     static Database? _database;

     Future<Database> get database async {
       if (_database != null) return _database!;
       _database = await _initDatabase();
       return _database!;
     }

     Future<Database> _initDatabase() async {
       final path = join(await getDatabasesPath(), 'app.db');
       return await openDatabase(
         path,
         version: 1,
         onCreate: (db, version) async {
           await db.execute('''
             CREATE TABLE users (
               id INTEGER PRIMARY KEY,
               name TEXT NOT NULL,
               email TEXT
             )
           ''');
         },
       );
     }

     Future<int> insertUser(User user) async {
       final db = await database;
       return await db.insert('users', user.toJson());
     }

     Future<List<User>> getUsers() async {
       final db = await database;
       final maps = await db.query('users');
       return maps.map((map) => User.fromJson(map)).toList();
     }
   }

.. tip::

   对于复杂数据库需求，考虑使用 **drift**（原 Moor）或 **ObjectBox**，
   它们提供了基于类型安全的 ORM。

网络状态与缓存
===============

.. code-block:: dart

   import 'dart:io';
   import 'package:connectivity_plus/connectivity_plus.dart';

   // 检查网络连接
   Future<bool> isConnected() async {
     try {
       final result = await InternetAddress.lookup('google.com');
       return result.isNotEmpty && result[0].rawAddress.isNotEmpty;
     } on SocketException catch (_) {
       return false;
     }
   }

   // 监听网络变化
   Connectivity().onConnectivityChanged.listen((result) {
     // 处理网络状态变化
   });

.. seealso::

   - 缓存策略：结合 ``dio_cache_interceptor`` 实现离线缓存
   - 状态同步：结合 ``Riverpod`` 或 ``Bloc`` 管理异步数据加载状态
   - 本地数据库：**drift** (类型安全)、**ObjectBox** (高性能)、**Hive** (轻量级)
