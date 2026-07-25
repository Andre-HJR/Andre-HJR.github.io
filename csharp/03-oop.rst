================
面向对象编程
================

类与对象
=========

.. code-block:: csharp

   public class Person
   {
       // 字段
       private string _name;
       private int _age;

       // 属性（推荐用属性替代公有字段）
       public string Name
       {
           get => _name;
           set => _name = value ?? throw new ArgumentNullException();
       }

       public int Age
       {
           get => _age;
           set => _age = value >= 0 ? value : 0;
       }

       // 自动属性（编译器自动生成字段）
       public string Email { get; set; }

       // 只读属性
       public bool IsAdult => Age >= 18;

       // 构造函数
       public Person(string name, int age)
       {
           Name = name;
           Age = age;
       }

       // 方法
       public virtual void Introduce()
       {
           Console.WriteLine($"Hi, I'm {Name}, {Age} years old.");
       }

       // 静态成员
       public static Person CreateDefault()
           => new Person("Guest", 0);

       public override string ToString()
           => $"Person(Name={Name}, Age={Age})";
   }

   // 使用
   var alice = new Person("Alice", 30);
   alice.Introduce();

record 类型（C# 9+）
======================

不可变数据模型的首选：

.. code-block:: csharp

   // record = 值语义的引用类型
   public record Person(string Name, int Age);

   // 使用
   var p1 = new Person("Alice", 30);
   var p2 = new Person("Alice", 30);

   Console.WriteLine(p1 == p2);              // true（值比较）
   Console.WriteLine(p1);                     // Person { Name = Alice, Age = 30 }

   var p3 = p1 with { Age = 31 };            // with 表达式（非破坏性修改）

   // record struct（值类型 record）
   public readonly record struct Point(int X, int Y);

.. tip::

   对于 DTO、配置模型、事件等不可变数据，**优先使用 record**。

继承与多态
============

.. code-block:: csharp

   public class Animal
   {
       public virtual void MakeSound()
           => Console.WriteLine("...");
   }

   public class Dog : Animal
   {
       public override void MakeSound()
           => Console.WriteLine("Woof!");
   }

   public class Cat : Animal
   {
       public override void MakeSound()
           => Console.WriteLine("Meow!");
   }

   // 多态
   Animal[] animals = { new Dog(), new Cat() };
   foreach (var animal in animals)
       animal.MakeSound();     // Woof! Meow!

抽象类与接口
==============

.. code-block:: csharp

   // 抽象类
   public abstract class Shape
   {
       public abstract double GetArea();
       public virtual void Draw()
           => Console.WriteLine("Drawing shape");
   }

   public class Circle : Shape
   {
       public double Radius { get; }
       public Circle(double radius) => Radius = radius;

       public override double GetArea()
           => Math.PI * Radius * Radius;
   }

   // 接口（C# 8+ 支持默认实现）
   public interface IComparable<T>
   {
       int CompareTo(T other);  // 抽象方法
   }

   public interface ILogger
   {
       void Log(string message);
       void LogError(string message)  // 默认实现
           => Log($"[ERROR] {message}");
   }

.. important::

   **抽象类 vs 接口：**

   - 抽象类：有共享状态（字段）、部分实现、"is-a" 关系
   - 接口：无状态、纯行为契约、"can-do" 关系
   - 一个类只能继承一个抽象类，但可实现多个接口

委托与事件
============

.. code-block:: csharp

   // 委托定义
   public delegate void NotifyHandler(string message);

   // 事件发布者
   public class Button
   {
       // 事件声明
       public event EventHandler? Clicked;
       public event EventHandler<string>? TextChanged;

       public void SimulateClick()
           => Clicked?.Invoke(this, EventArgs.Empty);
   }

   // 事件订阅者
   public class UIHandler
   {
       public void Subscribe()
       {
           var btn = new Button();
           btn.Clicked += OnButtonClicked;      // 订阅
           btn.Clicked += (s, e) => Console.WriteLine("匿名处理");
       }

       private void OnButtonClicked(object? sender, EventArgs e)
           => Console.WriteLine("按钮被点击！");
   }

   // Func / Action 内置委托
   Func<int, int, int> add = (a, b) => a + b;
   Action<string> log = msg => Console.WriteLine(msg);

泛型
======

.. code-block:: csharp

   // 泛型类
   public class Repository<T> where T : class
   {
       private readonly List<T> _items = new();

       public void Add(T item) => _items.Add(item);
       public T? GetById(int id) => _items.ElementAtOrDefault(id);
   }

   // 泛型方法
   public T? Find<T>(IEnumerable<T> source, Func<T, bool> predicate)
       => source.FirstOrDefault(predicate);

   // 泛型约束
   public class DataStore<T> where T : struct          // 值类型约束
   public class Service<T> where T : new()             // 无参构造函数约束
   public class Processor<T> where T : IComparable<T>  // 接口约束
