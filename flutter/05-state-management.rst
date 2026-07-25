================
状态管理
================

状态管理是 Flutter 应用的核心问题。随着应用规模增长，合理的状态管理方案至关重要。

什么是状态
===========

**状态（State）** 是会影响 UI 渲染的任何数据。

.. code-block::

   状态类型
   ├── 本地（Ephemeral）状态：单个 Widget 内使用
   │   └── TextField 的当前输入、当前页码
   └── 应用（App）状态：多个模块共享
       └── 登录信息、购物车、主题设置、语言偏好

.. tip::

   用 "是否需要多个 Widget 共享" 来选择方案：
   - 仅本 Widget 使用 → ``StatefulWidget`` + ``setState``
   - 需要共享 → 选择一种全局状态管理方案

setState（基础方案）
=====================

适合纯本地状态：

.. code-block:: dart

   class CounterPage extends StatefulWidget {
     @override
     State<CounterPage> createState() => _CounterPageState();
   }

   class _CounterPageState extends State<CounterPage> {
     int _count = 0;

     void _increment() {
       setState(() {
         _count++;
       });
     }

     @override
     Widget build(BuildContext context) {
       return Scaffold(
         body: Center(child: Text('$_count')),
         floatingActionButton: FloatingActionButton(
           onPressed: _increment,
           child: const Icon(Icons.add),
         ),
       );
     }
   }

.. caution::

   ``setState`` 在 Widget 树变大时性能下降。如果多层级间需要传递状态，
   应避免"逐层回调"的方式。

InheritedWidget（框架级）
=========================

Flutter 框架内置的共享状态机制：

.. code-block:: dart

   class ThemeProvider extends InheritedWidget {
     const ThemeProvider({
       super.key,
       required this.isDarkMode,
       required super.child,
     });

     final bool isDarkMode;

     static ThemeProvider of(BuildContext context) {
       return context.dependOnInheritedWidgetOfExactType<ThemeProvider>()!;
     }

     @override
     bool updateShouldNotify(ThemeProvider oldWidget) {
       return isDarkMode != oldWidget.isDarkMode;
     }
   }

Provider（推荐入门）
====================

Provider 是官方推荐的基础状态管理方案，封装了 InheritedWidget：

添加依赖
---------

.. code-block:: yaml
   :caption: pubspec.yaml

   dependencies:
     provider: ^6.0.0

定义模型
---------

.. code-block:: dart

   class CounterModel extends ChangeNotifier {
     int _count = 0;
     int get count => _count;

     void increment() {
       _count++;
       notifyListeners();   // 通知监听者重建 UI
     }
   }

注入与消费
-----------

.. code-block:: dart

   // 注入（通常在 main.dart）
   void main() {
     runApp(
       ChangeNotifierProvider(
         create: (_) => CounterModel(),
         child: const MyApp(),
       ),
     );
   }

   // 消费（在 Widget 中）
   class CounterPage extends StatelessWidget {
     @override
     Widget build(BuildContext context) {
       final counter = context.watch<CounterModel>();  // 监听变化
       return Scaffold(
         body: Center(child: Text('${counter.count}')),
         floatingActionButton: FloatingActionButton(
           onPressed: () => context.read<CounterModel>().increment(),
           child: const Icon(Icons.add),
         ),
       );
     }
   }

.. important::

   - ``context.watch<T>()``：当 T 变化时重建当前 Widget
   - ``context.read<T>()``：获取 T 但不监听变化

Riverpod（进阶推荐）
=====================

Riverpod 是 Provider 的升级版，编译安全、独立于 Widget 树。

添加依赖
---------

.. code-block:: yaml
   :caption: pubspec.yaml

   dependencies:
     flutter_riverpod: ^2.0.0

定义与使用
-----------

.. code-block:: dart

   // 定义 Provider
   final counterProvider = StateProvider<int>((ref) => 0);

   // 异步 Provider
   final userProvider = FutureProvider<User>((ref) async {
     return fetchUser();
   });

   // 消费
   class CounterWidget extends ConsumerWidget {
     @override
     Widget build(BuildContext context, WidgetRef ref) {
       final count = ref.watch(counterProvider);
       return ElevatedButton(
         onPressed: () => ref.read(counterProvider.notifier).state++,
         child: Text('$count'),
       );
     }
   }

.. tip::

   Riverpod 相比 Provider 的优势：
   - 无需 BuildContext 即可访问状态
   - 编译时检查（不会出现 ProviderNotFoundException）
   - 支持自动取消（autoDispose）
   - 更好的测试支持

Bloc（事件驱动）
=================

Bloc 基于事件驱动模型，适合复杂业务逻辑。

添加依赖
---------

.. code-block:: yaml
   :caption: pubspec.yaml

   dependencies:
     flutter_bloc: ^8.0.0

定义 Bloc
----------

.. code-block:: dart

   // 事件
   abstract class CounterEvent {}
   class Increment extends CounterEvent {}
   class Decrement extends CounterEvent {}

   // Bloc
   class CounterBloc extends Bloc<CounterEvent, int> {
     CounterBloc() : super(0) {
       on<Increment>((event, emit) => emit(state + 1));
       on<Decrement>((event, emit) => emit(state - 1));
     }
   }

使用 Bloc
----------

.. code-block:: dart

   // 注入
   BlocProvider(
     create: (_) => CounterBloc(),
     child: const CounterPage(),
   )

   // 消费
   BlocBuilder<CounterBloc, int>(
     builder: (context, count) {
       return Text('$count');
     },
   )

状态管理方案对比
=================

+----------------+--------------+---------------+-------------+---------------+
| 方案           | 复杂度       | 学习曲线      | 适用规模    | 核心概念      |
+================+==============+===============+=============+===============+
| setState       | 极低         | ★☆☆☆☆         | 极小型      | 本地状态      |
+----------------+--------------+---------------+-------------+---------------+
| Provider       | 低           | ★★☆☆☆         | 中小型      | ChangeNotifier|
+----------------+--------------+---------------+-------------+---------------+
| Riverpod       | 中           | ★★★☆☆         | 中大型      | Provider/Ref  |
+----------------+--------------+---------------+-------------+---------------+
| Bloc           | 高           | ★★★★☆         | 大型        | Event/State   |
+----------------+--------------+---------------+-------------+---------------+
| GetX           | 中           | ★★☆☆☆         | 中大型      | Reactive      |
+----------------+--------------+---------------+-------------+---------------+

.. note::

   对于新项目，推荐从 **Provider** 或 **Riverpod** 开始。
   当业务逻辑足够复杂、需要明确的事件流管理时，再引入 Bloc。
