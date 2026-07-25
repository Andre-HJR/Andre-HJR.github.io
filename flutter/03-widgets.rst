==============
Widget 体系
==============

Flutter 中 **一切皆 Widget**——从结构元素到样式修饰，Widget 是 UI 的基本构建块。

Widget 哲学
=============

核心原则
---------

1. **声明式 UI**：描述界面"应该是什么样子"，而非如何构建
2. **组合优于继承**：通过嵌套小型 Widget 构建复杂界面
3. **Widget 是不可变的**：每次变化都创建新的 Widget 实例

.. code-block:: dart

   // 声明式描述：一个居中的文本
   Scaffold(
     appBar: AppBar(title: Text('标题')),
     body: Center(
       child: Text('Hello, Flutter!'),
     ),
   )

StatelessWidget vs StatefulWidget
===================================

StatelessWidget（无状态）
--------------------------

界面一旦构建就不需要变化：

.. code-block:: dart

   class GreetingWidget extends StatelessWidget {
     const GreetingWidget({super.key, required this.name});
     final String name;

     @override
     Widget build(BuildContext context) {
       return Text('Hello, $name!');
     }
   }

StatefulWidget（有状态）
-------------------------

界面需要根据状态变化更新：

.. code-block:: dart

   class CounterWidget extends StatefulWidget {
     const CounterWidget({super.key});

     @override
     State<CounterWidget> createState() => _CounterWidgetState();
   }

   class _CounterWidgetState extends State<CounterWidget> {
     int _count = 0;

     void _increment() {
       setState(() => _count++);
     }

     @override
     Widget build(BuildContext context) {
       return Column(
         children: [
           Text('Count: $_count'),
           ElevatedButton(
             onPressed: _increment,
             child: const Text('+1'),
           ),
         ],
       );
     }
   }

.. important::

   ``setState`` 通知 Flutter 框架重新构建 Widget。不要直接修改状态而不调用它。

基础 Widget
============

文本显示
---------

.. code-block:: dart

   Text(
     'Hello, Flutter!',
     style: TextStyle(
       fontSize: 24,
       fontWeight: FontWeight.bold,
       color: Colors.blue,
       letterSpacing: 1.2,
     ),
     textAlign: TextAlign.center,
     maxLines: 2,
     overflow: TextOverflow.ellipsis,
   )

   // 富文本
   Text.rich(
     TextSpan(
       text: '普通文本 ',
       children: [
         TextSpan(
           text: '粗体',
           style: TextStyle(fontWeight: FontWeight.bold),
         ),
         TextSpan(
           text: ' 带颜色',
           style: TextStyle(color: Colors.red),
         ),
       ],
     ),
   )

图片
-----

.. code-block:: dart

   // 本地图片
   Image.asset('assets/images/logo.png'),

   // 网络图片
   Image.network(
     'https://example.com/image.jpg',
     width: 200,
     height: 200,
     fit: BoxFit.cover,
     loadingBuilder: (context, child, progress) {
       if (progress == null) return child;
       return CircularProgressIndicator();
     },
     errorBuilder: (context, error, stack) =>
       const Icon(Icons.error),
   )

按钮
-----

.. code-block:: dart

   // 凸起按钮
   ElevatedButton(
     onPressed: () => print('点击'),
     child: const Text('确定'),
   )

   // 文本按钮
   TextButton(
     onPressed: () {},
     child: const Text('取消'),
   )

   // 描边按钮
   OutlinedButton(
     onPressed: () {},
     child: const Text('更多'),
   )

   // 图标按钮
   IconButton(
     onPressed: () {},
     icon: const Icon(Icons.favorite),
   )

输入框
-------

.. code-block:: dart

   TextField(
     controller: _controller,           // 可选：TextEditingController
     decoration: InputDecoration(
       labelText: '用户名',
       hintText: '请输入用户名',
       prefixIcon: Icon(Icons.person),
       border: OutlineInputBorder(),
     ),
     obscureText: true,                 // 密码模式
     keyboardType: TextInputType.emailAddress,
     onChanged: (value) => print(value),
     validator: (value) {
       if (value == null || value.isEmpty) {
         return '不能为空';
       }
       return null;
     },
   )

容器与修饰
===========

Container
----------

.. code-block:: dart

   Container(
     width: 200,
     height: 200,
     margin: EdgeInsets.all(16),
     padding: EdgeInsets.symmetric(horizontal: 8, vertical: 4),
     decoration: BoxDecoration(
       color: Colors.white,
       borderRadius: BorderRadius.circular(12),
       boxShadow: [
         BoxShadow(
           color: Colors.black.withOpacity(0.1),
           blurRadius: 8,
           offset: Offset(0, 2),
         ),
       ],
       border: Border.all(color: Colors.grey[300]!),
     ),
     child: const Text('带样式容器'),
   )

SizedBox（间隔控件）
--------------------

.. code-block:: dart

   const SizedBox(height: 16),        // 垂直间距
   const SizedBox(width: 8),          // 水平间距
   SizedBox.shrink(),                 // 零大小
   SizedBox.expand(child: widget),    // 撑满可用空间

Padding
-------

.. code-block:: dart

   const Padding(
     padding: EdgeInsets.all(16.0),
     child: Text('有内边距的文本'),
   )

Material 与 Cupertino 组件
============================

Material Design（Android 风格）
---------------------------------

.. code-block:: dart

   Scaffold(
     appBar: AppBar(
       title: const Text('页面标题'),
       actions: [
         IconButton(icon: const Icon(Icons.search), onPressed: () {}),
       ],
     ),
     body: ListView(
       children: [
         ListTile(
           leading: CircleAvatar(child: Text('A')),
           title: const Text('联系人'),
           subtitle: const Text('在线'),
           trailing: Icon(Icons.chevron_right),
           onTap: () {},
         ),
       ],
     ),
     bottomNavigationBar: BottomNavigationBar(
       items: const [
         BottomNavigationBarItem(icon: Icon(Icons.home), label: '首页'),
         BottomNavigationBarItem(icon: Icon(Icons.settings), label: '设置'),
       ],
     ),
     floatingActionButton: FloatingActionButton(
       onPressed: () {},
       child: const Icon(Icons.add),
     ),
   )

Cupertino（iOS 风格）
-----------------------

.. code-block:: dart

   import 'package:flutter/cupertino.dart';

   CupertinoPageScaffold(
     navigationBar: CupertinoNavigationBar(
       middle: const Text('iOS 页面'),
       trailing: CupertinoButton(
         onPressed: () {},
         child: const Icon(CupertinoIcons.settings),
       ),
     ),
     child: Center(
       child: CupertinoButton.filled(
         child: const Text('iOS 按钮'),
         onPressed: () {},
       ),
     ),
   )

.. tip::

   在 MaterialApp 中设置 ``theme: ThemeData(useMaterial3: true)``
   以启用 Material 3 (You) 设计语言，获得更现代的视觉效果。
