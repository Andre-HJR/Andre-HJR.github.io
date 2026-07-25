============
布局与样式
============

Flutter 的布局系统基于 **组件树** 结构，每个 Widget 负责自己的布局约束。

布局模型
=========

Flutter 布局三步骤
-------------------

#. **传递约束** (父 -> 子)：父 Widget 告诉子 Widget 最大/最小宽高
#. **确定尺寸** (子 -> 父)：子 Widget 根据约束确定自己的尺寸
#. **摆放位置** (父)：父 Widget 在自身区域摆放子 Widget

.. code-block::

   父 Widget
      │   "你的最大宽度是 300，最小是 0"
      ▼
   子 Widget  ──→  "我选宽度 200，高度 100"
                     │
                     ▼
                父将子摆放在 x=50, y=10 位置

核心布局 Widget
=================

Row（水平布局）
---------------

.. code-block:: dart

   Row(
     mainAxisAlignment: MainAxisAlignment.spaceEvenly,  // 主轴（水平）对齐
     crossAxisAlignment: CrossAxisAlignment.center,     // 交叉轴（垂直）对齐
     children: [
       Icon(Icons.star, size: 48),
       Icon(Icons.favorite, size: 48),
       Icon(Icons.thumb_up, size: 48),
     ],
   )

+--------------------------------+-----------------------------------------+
| MainAxisAlignment              | 效果                                    |
+================================+=========================================+
| ``start``                      | 从主轴起点开始排列                      |
+--------------------------------+-----------------------------------------+
| ``center``                     | 主轴居中                                |
+--------------------------------+-----------------------------------------+
| ``end``                        | 主轴末尾                                |
+--------------------------------+-----------------------------------------+
| ``spaceBetween``               | 两端对齐，中间间距均匀                  |
+--------------------------------+-----------------------------------------+
| ``spaceAround``                | 每个元素左右间距相等                    |
+--------------------------------+-----------------------------------------+
| ``spaceEvenly``                | 所有间距（含两端）相等                  |
+--------------------------------+-----------------------------------------+

Column（垂直布局）
------------------

.. code-block:: dart

   Column(
     mainAxisAlignment: MainAxisAlignment.center,
     crossAxisAlignment: CrossAxisAlignment.stretch,  // 子元素撑满交叉轴
     children: [
       Text('第一行'),
       Text('第二行'),
       Text('第三行'),
     ],
   )

布局与定位
============

Stack（层叠布局）
-----------------

类似 CSS 的绝对定位：

.. code-block:: dart

   Stack(
     children: [
       // 底部：背景图片
       Image.network('https://example.com/bg.jpg'),
       // 顶部：居中文字
       const Positioned(
         bottom: 20,
         left: 0,
         right: 0,
         child: Text(
           '图片底部文字',
           textAlign: TextAlign.center,
           style: TextStyle(color: Colors.white, fontSize: 18),
         ),
       ),
     ],
   )

Align（对齐）
--------------

.. code-block:: dart

   Align(
     alignment: Alignment.topRight,
     child: const FlutterLogo(size: 60),
   )

   // 分数定位（0 到 1 之间）
   Align(
     alignment: Alignment(0.5, 0.5),     // 正中心
     child: const Text('居中'),
   )

Flexible 与 Expanded
======================

在 Row 或 Column 中，用 Expanded 让子元素拉伸填充剩余空间：

.. code-block:: dart

   Row(
     children: [
       // 固定宽度
       Container(width: 50, color: Colors.red),
       // 占据剩余空间的 2/3（flex: 2 / (2+1)）
       Expanded(
         flex: 2,
         child: Container(color: Colors.green),
       ),
       // 占据剩余空间的 1/3
       Expanded(
         flex: 1,
         child: Container(color: Colors.blue),
       ),
     ],
   )

.. code-block:: dart

   // Flexible 允许子元素小于分配空间
   Flexible(
     flex: 1,
     child: Text('这是一段可能很长的文本，如果空间不够会换行'),
   )

.. tip::

   - ``Expanded`` = ``Flexible(fit: FlexFit.tight)``——强制填充分配空间
   - ``Flexible`` = ``Flexible(fit: FlexFit.loose)``——允许子元素更小

弹性与滚动
===========

SingleChildScrollView（单子滚动）
---------------------------------

.. code-block:: dart

   SingleChildScrollView(
     padding: const EdgeInsets.all(16),
     child: Column(
       children: [
         // 内容超出屏幕时自动可滚动
         for (int i = 0; i < 50; i++)
           ListTile(title: Text('Item $i')),
       ],
     ),
   )

ListView（列表）
-----------------

.. code-block:: dart

   // 简单列表
   ListView(
     children: const [
       ListTile(title: Text('Item 1')),
       ListTile(title: Text('Item 2')),
       Divider(),
     ],
   )

   // 高性能长列表（按需构建）
   ListView.builder(
     itemCount: 1000,
     itemBuilder: (context, index) {
       return ListTile(
         leading: CircleAvatar(child: Text('$index')),
         title: Text('Item $index'),
       );
     },
   )

GridView（网格）
-----------------

.. code-block:: dart

   // 固定交叉轴数量
   GridView.count(
     crossAxisCount: 2,
     children: [
       for (int i = 0; i < 20; i++)
         Card(child: Center(child: Text('Card $i'))),
     ],
   )

   // 自适应列宽
   GridView.extent(
     maxCrossAxisExtent: 200,    // 每列最大宽度
     children: [
       for (int i = 0; i < 20; i++)
         Card(child: Center(child: Text('Card $i'))),
     ],
   )

自适应布局
============

LayoutBuilder（获取父约束）
---------------------------

.. code-block:: dart

   LayoutBuilder(
     builder: (context, constraints) {
       if (constraints.maxWidth > 600) {
         return _buildWideLayout();
       } else {
         return _buildNarrowLayout();
       }
     },
   )

MediaQuery（屏幕信息）
----------------------

.. code-block:: dart

   // 获取屏幕宽度
   final screenWidth = MediaQuery.of(context).size.width;

   // 响应式判断
   final isSmall = screenWidth < 360;
   final isTablet = screenWidth > 768;

   // 安全区域（刘海屏、圆角）
   final padding = MediaQuery.of(context).padding;

尺寸与间距速查
===============

.. list-table::
   :header-rows: 1

   * - Widget
     - 用途
   * - ``SizedBox(width: x, height: y)``
     - 固定尺寸的空白或约束子元素
   * - ``ConstrainedBox``
     - 施加额外的最大/最小约束
   * - ``FractionallySizedBox``
     - 按父容器的比例确定尺寸
   * - ``AspectRatio``
     - 强制宽高比 (如 16:9)
   * - ``LimitedBox``
     - 仅在无约束时施加约束
