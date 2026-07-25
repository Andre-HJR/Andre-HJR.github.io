============
并发编程
============

Scala 提供了从底层到高层的多种并发编程工具。

线程基础
=========

Scala 直接使用 Java 的线程模型：

.. code-block:: scala

   // 创建线程
   val thread = new Thread:
     override def run(): Unit =
       println(s"线程 ${Thread.currentThread.getName} 执行中")

   thread.start()
   thread.join()   // 等待线程结束

.. warning::

   直接操作线程容易出错（竞态条件、死锁等）。推荐使用更高层的抽象。

Future
=======

``Future`` 代表一个**异步计算**的结果，是 Scala 并发编程的核心抽象。

基本用法
---------

.. code-block:: scala

   import scala.concurrent.{Future, Await}
   import scala.concurrent.ExecutionContext.Implicits.global
   import scala.concurrent.duration.*

   // 创建一个异步任务
   val future: Future[Int] = Future:
     Thread.sleep(1000)
     42

   // 回调方式获取结果
   future.onComplete:
     case scala.util.Success(value) => println(s"结果: $value")
     case scala.util.Failure(e)     => println(s"失败: ${e.getMessage}")

   // 阻塞等待（尽量少用）
   val result = Await.result(future, 3.seconds)    // 42

.. note::

   ``ExecutionContext`` 隐式提供线程池。默认的 ``global`` 使用 ForkJoinPool。

Future 链式操作
-----------------

.. code-block:: scala

   case class User(id: Int, name: String)
   case class Order(id: Int, amount: Double)

   def fetchUser(id: Int): Future[User] = Future:
     Thread.sleep(500); User(id, "Alice")

   def fetchOrders(user: User): Future[List[Order]] = Future:
     Thread.sleep(500); List(Order(1, 99.9))

   // === map: 转换 Future 内的值 ===
   fetchUser(1).map(_.name).foreach(println)   // "Alice"

   // === flatMap: 链式异步操作 ===
   val orderTotal: Future[Double] =
     fetchUser(1).flatMap: user =>
       fetchOrders(user).map: orders =>
         orders.map(_.amount).sum

   // === for 推导式语法糖 ===
   val total: Future[Double] = for
     user   <- fetchUser(1)
     orders <- fetchOrders(user)
   yield orders.map(_.amount).sum

Future 组合
------------

.. code-block:: scala

   val f1 = Future(100)
   val f2 = Future(200)

   // 合并两个 Future
   val combined: Future[Int] = f1.zip(f2).map((a, b) => a + b)

   // 等待所有完成
   val futures = List(f1, f2, Future(300))
   val allDone: Future[List[Int]] = Future.sequence(futures)

   // 先完成者获胜
   val firstDone: Future[Int] = Future.firstCompletedOf(futures)

Promise
========

``Promise`` 是 ``Future`` 的可写端——你可以手动完成一个 Promise：

.. code-block:: scala

   import scala.concurrent.Promise

   val promise = Promise[String]()

   // 在其他线程完成 Promise
   Future:
     Thread.sleep(1000)
     promise.success("完成!")

   // 获取对应的 Future
   val future = promise.future
   future.foreach(println)   // 1 秒后输出 "完成!"

.. tip::

   Promise 适合需要手动控制完成时机的场景（如回调转 Future 的桥接）。

ExecutionContext 与自定义线程池
================================

.. code-block:: scala

   import java.util.concurrent.{Executors, ForkJoinPool}
   import scala.concurrent.{ExecutionContext, Future}

   // 1. 固定大小线程池
   val threadPool = Executors.newFixedThreadPool(4)
   given ec: ExecutionContext = ExecutionContext.fromExecutor(threadPool)

   // 2. 自定义 ForkJoinPool
   given fjp: ExecutionContext =
     ExecutionContext.fromExecutor(ForkJoinPool(4))

   Future(compute())(ec)   // 显式传入 ExecutionContext

.. caution::

   使用完线程池记得关闭：``threadPool.shutdown()``。

Akka 与 Actor 模型
====================

Actor 模型将"共享状态"转变为"消息传递"，每个 Actor 维护自己的私有状态。

基本 Actor（Akka Classic）
---------------------------

.. code-block:: scala

   import akka.actor.{Actor, ActorSystem, Props}

   class HelloActor extends Actor:
     def receive: Receive =
       case "hello" => println("你好!")
       case name: String => println(s"你好, $name!")
       case _ => println("无法理解")

   val system = ActorSystem("MySystem")
   val helloActor = system.actorOf(Props[HelloActor](), "hello-actor")
   helloActor ! "hello"     // 发送消息（异步，fire-and-forget）

Akka Typed（类型安全 Actor）
-----------------------------

.. code-block:: scala

   import akka.actor.typed.{ActorRef, ActorSystem, Behavior}
   import akka.actor.typed.scaladsl.Behaviors

   object Greeter:
     sealed trait Command
     case class Greet(name: String, replyTo: ActorRef[String]) extends Command

     def apply(): Behavior[Command] = Behaviors.receiveMessage:
       case Greet(name, replyTo) =>
         replyTo ! s"你好, $name!"   // 发送回复
         Behaviors.same

   val system = ActorSystem(Greeter(), "greeter")

.. warning::

   Akka 是一个大型框架。对于简单并发需求，Future 已经足够；
   Actor 模型更适合复杂的状态管理和分布式场景。

cats.effect.IO 与 ZIO
======================

函数式并发库提供比 Future 更好的可组合性和类型安全性。

cats.effect.IO
--------------

.. code-block:: scala

   import cats.effect.{IO, IOApp}

   object MyApp extends IOApp.Simple:
     def run: IO[Unit] =
       for
         _ <- IO.println("Hello, cats.effect!")
         _ <- IO.sleep(1.second)
         _ <- IO.println("1 秒后...")
       yield ()

ZIO
----

.. code-block:: scala

   import zio.*

   object MyApp extends ZIOAppDefault:
     def run =
       for
         _ <- Console.printLine("Hello, ZIO!")
         _ <- ZIO.sleep(1.second)
         _ <- Console.printLine("1 秒后...")
       yield ()

.. seealso::

   - cats.effect: https://typelevel.org/cats-effect/
   - ZIO: https://zio.dev/

选择指南
=========

+----------------+-----------------------------------+------------------------------------+
| 工具           | 适用场景                          | 不适用场景                         |
+================+===================================+====================================+
| ``Future``     | 标准的一次性异步任务              | 需要取消或资源回收的场景           |
+----------------+-----------------------------------+------------------------------------+
| Akka Actor     | 复杂状态机、分布式系统、流处理    | 简单的"发请求-等响应"场景          |
+----------------+-----------------------------------+------------------------------------+
| cats.effect    | 纯函数式应用、资源安全的 I/O      | 团队不熟悉 FP 的场合               |
+----------------+-----------------------------------+------------------------------------+
| ZIO            | 大规模企业级函数式系统            | 小型项目（引入成本高）             |
+----------------+-----------------------------------+------------------------------------+
