==============
路由与导航
==============

导航（Navigation）是移动应用的基本能力。Flutter 提供了灵活的路由系统。

基本路由
=========

Navigator.push / pop
---------------------

最直接的路由方式：

.. code-block:: dart

   // 跳转到新页面
   Navigator.push(
     context,
     MaterialPageRoute(
       builder: (context) => const DetailPage(),
     ),
   );

   // 返回上一页
   Navigator.pop(context);

.. code-block:: dart

   // 页面间传参
   class DetailPage extends StatelessWidget {
     const DetailPage({super.key, required this.itemId});
     final int itemId;

     @override
     Widget build(BuildContext context) {
       return Scaffold(
         appBar: AppBar(title: Text('Item $itemId')),
         body: Center(
           child: ElevatedButton(
             onPressed: () => Navigator.pop(context, '返回的数据'),
             child: const Text('返回'),
           ),
         ),
       );
     }
   }

   // 接收返回值
   final result = await Navigator.push<String>(
     context,
     MaterialPageRoute(
       builder: (context) => const DetailPage(itemId: 42),
     ),
   );
   print('返回结果: $result');   // "返回的数据"

命名路由
=========

在 MaterialApp 中预先注册路由：

.. code-block:: dart

   MaterialApp(
     initialRoute: '/',
     routes: {
       '/':       (context) => const HomePage(),
       '/detail': (context) => const DetailPage(),
       '/about':  (context) => const AboutPage(),
     },
   )

   // 使用命名路由导航
   Navigator.pushNamed(context, '/detail');
   Navigator.pushNamed(context, '/about');

   // 返回上一页
   Navigator.pop(context);

.. warning::

   命名路由不支持传递运行时参数（如动态 ID）。
   如需传参，使用 ``onGenerateRoute`` 或 ``Navigator.push``。

onGenerateRoute（动态路由）
============================

更灵活的路由方案，支持参数传递：

.. code-block:: dart

   MaterialApp(
     onGenerateRoute: (settings) {
       // 解析路由名称与参数
       final uri = Uri.parse(settings.name ?? '');
       final path = uri.path;
       final queryParams = uri.queryParameters;

       switch (path) {
         case '/':
           return MaterialPageRoute(
             builder: (_) => const HomePage(),
           );
         case '/detail':
           final id = int.tryParse(queryParams['id'] ?? '0') ?? 0;
           return MaterialPageRoute(
             builder: (_) => DetailPage(itemId: id),
           );
         default:
           return MaterialPageRoute(
             builder: (_) => const NotFoundPage(),
           );
       }
     },
   )

.. code-block:: dart

   // 传递参数
   Navigator.pushNamed(context, '/detail?id=42');
   // 或
   Navigator.pushNamed(context, '/detail', arguments: {'id': 42});

命名路由中的参数提取
----------------------

.. code-block:: dart

   // 在目标页面提取参数
   class DetailPage extends StatelessWidget {
     const DetailPage({super.key});

     @override
     Widget build(BuildContext context) {
       // 方式 1：从 RouteSettings 取
       final args =
           ModalRoute.of(context)!.settings.arguments as Map<String, dynamic>;
       final id = args['id'] as int;

       // 方式 2：从路径解析
       // final id = int.tryParse(
       //   ModalRoute.of(context)!.settings.name?.split('?').last ?? '0'
       // ) ?? 0;

       return Scaffold(
         appBar: AppBar(title: Text('Detail $id')),
       );
     }
   }

GoRouter（声明式路由）
=======================

GoRouter 是 Flutter 官方推荐的声明式路由方案，支持深度链接和重定向。

添加依赖
---------

.. code-block:: yaml
   :caption: pubspec.yaml

   dependencies:
     go_router: ^14.0.0

基本用法
---------

.. code-block:: dart

   import 'package:go_router/go_router.dart';

   // 定义路由配置
   final router = GoRouter(
     initialLocation: '/',
     routes: [
       GoRoute(
         path: '/',
         name: 'home',
         builder: (context, state) => const HomePage(),
       ),
       GoRoute(
         path: '/detail/:id',          // 路径参数
         name: 'detail',
         builder: (context, state) {
           final id = state.pathParameters['id']!;
           return DetailPage(itemId: int.parse(id));
         },
         routes: [
           // 嵌套路由：/detail/:id/info
           GoRoute(
             path: 'info',
             builder: (context, state) => const InfoPage(),
           ),
         ],
       ),
       GoRoute(
         path: '/search',
         name: 'search',
         builder: (context, state) => const SearchPage(),
       ),
     ],
   );

   // 在 MaterialApp.router 中使用
   MaterialApp.router(
     routerConfig: router,
     title: 'My App',
   );

导航方法
---------

.. code-block:: dart

   // 跳转
   context.go('/detail/42');
   context.push('/detail/42');
   context.pushNamed('detail', pathParameters: {'id': '42'});

   // 返回
   context.pop();

   // 带查询参数
   context.go('/search?q=flutter&page=1');

   // 替换当前路由（不保留返回历史）
   context.replace('/detail/99');

重定向与守卫
-------------

.. code-block:: dart

   final router = GoRouter(
     redirect: (context, state) {
       final loggedIn = _isLoggedIn;
       final onLogin = state.matchedLocation == '/login';

       // 未登录且不在登录页 → 跳转到登录
       if (!loggedIn && !onLogin) return '/login';

       // 已登录且在登录页 → 跳转到首页
       if (loggedIn && onLogin) return '/';

       // 不重定向
       return null;
     },
     // ...
   );

.. tip::

   GoRouter 的优势：
   - **声明式**：路由配置集中管理
   - **路径参数**：类似 Web 路由的 ``:id`` 语法
   - **深度链接**：自动支持 Android/iOS 的 URL Scheme
   - **重定向**：统一处理登录认证等守卫逻辑
   - **嵌套路由**：父子路由关系清晰

路由方案选择
=============

+------------------------+-----------------------------+-------------------------------+
| 方案                   | 适用场景                    | 不推荐场景                    |
+========================+=============================+===============================+
| Navigator.push/pop     | 简单的 2-3 个页面跳转       | 页面多、需要深度链接          |
+------------------------+-----------------------------+-------------------------------+
| 命名路由               | 页面固定且无需传参          | 需要动态参数或复杂路由        |
+------------------------+-----------------------------+-------------------------------+
| onGenerateRoute        | 需要集中控制页面创建        | 嵌套路由需求复杂              |
+------------------------+-----------------------------+-------------------------------+
| **GoRouter**           | 中大型应用、Web 端、深度链接| 仅 1-2 页面的极简应用         |
+------------------------+-----------------------------+-------------------------------+
