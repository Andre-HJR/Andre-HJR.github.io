====================
C# 高级特性
====================

LINQ（语言集成查询）
======================

LINQ 是 C# 中最强大的特性之一，提供统一的集合查询方式。

.. code-block:: csharp

   var numbers = new[] { 5, 2, 8, 1, 9, 3, 6, 4, 7 };

   // 方法语法（Fluent Syntax）
   var result = numbers
       .Where(n => n > 4)          // 筛选
       .OrderByDescending(n => n)  // 排序
       .Select(n => n * 10)        // 投影
       .ToList();                  // 物化

   // 查询语法（Query Syntax）
   var query = from n in numbers
               where n > 4
               orderby n descending
               select n * 10;

   // 常用 LINQ 操作符
   numbers.Where(n => n % 2 == 0);      // 筛选
   numbers.Select(n => n * 2);          // 投影
   numbers.OrderBy(n => n);             // 排序
   numbers.GroupBy(n => n % 2);         // 分组

   numbers.Any(n => n > 5);             // 任一满足？
   numbers.All(n => n > 0);             // 全部满足？
   numbers.Contains(5);                 // 包含？

   numbers.First();                     // 第一个
   numbers.Last();                      // 最后一个
   numbers.Single();                    // 唯一一个（多于 1 个抛异常）
   numbers.FirstOrDefault();            // 第一个或默认

   numbers.Skip(3).Take(2);             // 分页
   numbers.TakeWhile(n => n < 5);       // 条件取

   numbers.Sum();                       // 求和
   numbers.Average();                   // 平均
   numbers.Min();                       // 最小
   numbers.Max();                       // 最大
   numbers.Count();                     // 计数

.. tip::

   **LINQ 延迟执行：** 大多数 LINQ 操作是惰性的——直到遍历结果时才执行。
   使用 ``ToList()`` / ``ToArray()`` / ``ToDictionary()`` 强制物化。

async/await 异步编程
======================

.. code-block:: csharp

   // 异步方法定义
   public async Task<string> FetchDataAsync(string url)
   {
       using var client = new HttpClient();
       var response = await client.GetStringAsync(url);
       return response;
   }

   // 并发调用
   public async Task ProcessMultipleAsync()
   {
       var task1 = FetchDataAsync("https://api1.example.com");
       var task2 = FetchDataAsync("https://api2.example.com");

       // 等待所有完成
       var results = await Task.WhenAll(task1, task2);

       // 等待任意一个完成
       var first = await Task.WhenAny(task1, task2);
   }

   // 异步流（C# 8+）
   public async IAsyncEnumerable<int> GenerateSequenceAsync()
   {
       for (int i = 0; i < 10; i++)
       {
           await Task.Delay(100);
           yield return i;
       }
   }

   // 消费异步流
   await foreach (var number in GenerateSequenceAsync())
       Console.WriteLine(number);

.. important::

   async/await 最佳实践：

   - 异步方法命名以 ``Async`` 结尾
   - 避免 ``.Result`` / ``.Wait()`` ——可能导致死锁
   - 库方法用 ``ConfigureAwait(false)`` 避免上下文捕获
   - 异步方法内的 ``using`` 自动等待释放

模式匹配（C# 7+）
===================

.. code-block:: csharp

   // 类型模式
   object value = 42;
   if (value is int number)
       Console.WriteLine(number * 2);

   // 属性模式
   if (person is { Name: "Alice", Age: > 18 })
       Console.WriteLine("成年 Alice");

   // 位置模式（配合 deconstruct）
   if (point is (0, 0))
       Console.WriteLine("原点");

   // 列表模式（C# 11+）
   int[] numbers = { 1, 2, 3 };
   if (numbers is [1, .., 3])
       Console.WriteLine("以 1 开头, 3 结尾");

   // switch 表达式中综合使用
   static string Classify(object item) => item switch
   {
       int i and > 0 => $"正整数 {i}",
       int i and < 0 => $"负整数 {i}",
       string { Length: > 10 } s => $"长字符串: {s[..10]}...",
       null => "空值",
       _ => $"其他: {item.GetType().Name}"
   };

扩展方法
=========

.. code-block:: csharp

   // 为已有类型添加方法
   public static class StringExtensions
   {
       public static bool IsEmail(this string str)
           => Regex.IsMatch(str, @"^[\w.-]+@[\w.-]+\.\w+$");

       public static string Truncate(this string str, int maxLength)
           => str.Length <= maxLength ? str : str[..maxLength] + "...";
   }

   // 使用
   "alice@example.com".IsEmail();     // true
   "Hello World".Truncate(5);         // "Hello..."

.. tip::

   扩展方法**不能**访问被扩展类型的私有成员——本质上是静态方法的语法糖。

依赖注入（DI）
================

.. code-block:: csharp

   // 接口定义
   public interface IUserService
   {
       Task<User?> GetUserAsync(int id);
   }

   // 实现
   public class UserService : IUserService
   {
       private readonly HttpClient _httpClient;

       public UserService(HttpClient httpClient)
       {
           _httpClient = httpClient;  // DI 注入
       }

       public async Task<User?> GetUserAsync(int id)
       {
           return await _httpClient
               .GetFromJsonAsync<User>($"/users/{id}");
       }
   }

   // 注册服务（Program.cs）
   var builder = WebApplication.CreateBuilder(args);
   builder.Services.AddHttpClient<IUserService, UserService>();
   builder.Services.AddSingleton<ILogger, ConsoleLogger>();

反射与特性
============

.. code-block:: csharp

   // 自定义特性
   [AttributeUsage(AttributeTargets.Property)]
   public class DisplayNameAttribute : Attribute
   {
       public string Name { get; }
       public DisplayNameAttribute(string name) => Name = name;
   }

   public class UserModel
   {
       [DisplayName("用户 ID")]
       public int Id { get; set; }

       [DisplayName("用户名称")]
       public string Name { get; set; } = "";
   }

   // 反射读取特性
   public static string GetDisplayName<T>(string propertyName)
   {
       var prop = typeof(T).GetProperty(propertyName);
       var attr = prop?.GetCustomAttribute<DisplayNameAttribute>();
       return attr?.Name ?? propertyName;
   }

不安全代码与互操作
====================

.. code-block:: csharp

   // unsafe 代码（需在 csproj 中启用 AllowUnsafeBlocks）
   unsafe
   {
       int value = 42;
       int* ptr = &value;
       Console.WriteLine(*ptr);  // 42

       // 栈分配
       Span<int> buffer = stackalloc int[256];
   }

   // P/Invoke 调用 Win32 API
   [DllImport("user32.dll", CharSet = CharSet.Auto)]
   public static extern int MessageBox(
       IntPtr hWnd, string text, string caption, uint type);

   // 使用
   MessageBox(IntPtr.Zero, "Hello", "P/Invoke", 0);
