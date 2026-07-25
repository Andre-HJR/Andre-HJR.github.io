======================
.NET 框架核心
======================

.NET 基类库（BCL）
====================

BCL 提供所有 .NET 应用共享的核心功能。

System 命名空间
-----------------

.. code-block:: csharp

   // 数学运算
   Math.Max(10, 20);              // 20
   Math.Min(10, 20);              // 10
   Math.Abs(-5);                  // 5
   Math.Pow(2, 10);               // 1024
   Math.Round(3.14159, 2);        // 3.14
   Math.Clamp(value, 0, 100);     // 限制在 [0, 100]

   // 随机数
   var rng = Random.Shared;
   int dice = rng.Next(1, 7);     // 1~6
   double pct = rng.NextDouble();  // 0.0~1.0

   // 日期时间
   DateTime now = DateTime.Now;
   DateTime utc = DateTime.UtcNow;
   DateTime today = DateTime.Today;
   TimeSpan span = end - start;

   // 时间运算
   DateTime future = now.AddDays(7);
   TimeSpan diff = future - now;
   Console.WriteLine(diff.TotalHours);  // 168

   // GUID
   Guid id = Guid.NewGuid();
   Console.WriteLine(id.ToString());    // 全局唯一 ID

文件 I/O
=========

.. code-block:: csharp

   // 读写文本
   string content = File.ReadAllText("data.txt");
   string[] lines = File.ReadAllLines("data.txt");
   File.WriteAllText("output.txt", "Hello");

   // 异步版本
   string text = await File.ReadAllTextAsync("data.txt");
   await File.WriteAllTextAsync("output.txt", "Hello");

   // 流式处理
   using var reader = new StreamReader("large.txt");
   string? line;
   while ((line = await reader.ReadLineAsync()) != null)
       ProcessLine(line);

   // 文件/目录信息
   var info = new FileInfo("data.txt");
   Console.WriteLine(info.Length);    // 大小
   Console.WriteLine(info.CreationTime);

   var dir = new DirectoryInfo(@"C:\Data");
   foreach (var file in dir.GetFiles("*.txt"))
       Console.WriteLine(file.Name);

序列化
========

.. code-block:: csharp

   // System.Text.Json（.NET Core 3+ 内置）
   var user = new User { Id = 1, Name = "Alice" };

   // 序列化
   string json = JsonSerializer.Serialize(user);
   File.WriteAllText("user.json", json);

   // 反序列化
   string jsonText = File.ReadAllText("user.json");
   var deserialized = JsonSerializer.Deserialize<User>(jsonText);

   // 配置
   var options = new JsonSerializerOptions
   {
       PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
       WriteIndented = true,
       PropertyNameCaseInsensitive = true,
   };

   // XML 序列化（旧版兼容）
   var serializer = new XmlSerializer(typeof(User));
   using var writer = new StreamWriter("user.xml");
   serializer.Serialize(writer, user);

进程与线程
============

.. code-block:: csharp

   // Task（推荐的方式）
   Task task = Task.Run(() => DoWork());
   await task;

   Task<int> task2 = Task.Run(() => Compute());
   int result = await task2;

   // Parallel 循环
   Parallel.For(0, 100, i =>
   {
       Console.WriteLine($"并行处理 {i}");
   });

   // PLINQ
   var result = data.AsParallel()
                    .Where(x => x > 5)
                    .Select(x => ExpensiveOperation(x))
                    .ToList();

   // lock 同步
   private readonly object _lock = new();
   lock (_lock)
   {
       sharedCounter++;
   }

   // 信号量
   using var semaphore = new SemaphoreSlim(3);  // 最多 3 并发
   await semaphore.WaitAsync();
   try { await DoWork(); }
   finally { semaphore.Release(); }

配置管理
=========

.. code-block:: csharp

   // appsettings.json
   // {
   //   "ConnectionStrings": {
   //     "Default": "Server=.;Database=MyDb;..."
   //   },
   //   "Logging": { "Level": "Information" }
   // }

   // 读取配置
   var config = new ConfigurationBuilder()
       .SetBasePath(Directory.GetCurrentDirectory())
       .AddJsonFile("appsettings.json")
       .Build();

   string connStr = config.GetConnectionString("Default");
   string logLevel = config["Logging:Level"];

   // 强类型配置绑定
   public class DatabaseOptions
   {
       public string ConnectionString { get; set; } = "";
       public int TimeoutSeconds { get; set; } = 30;
   }

   var dbOptions = config
       .GetSection("Database")
       .Get<DatabaseOptions>();

日志系统
=========

.. code-block:: csharp

   // ILogger 接口（Microsoft.Extensions.Logging）
   public class OrderService
   {
       private readonly ILogger<OrderService> _logger;

       public OrderService(ILogger<OrderService> logger)
       {
           _logger = logger;
       }

       public async Task CreateOrderAsync(Order order)
       {
           _logger.LogInformation("创建订单：{OrderId}", order.Id);

           try
           {
               // 业务逻辑...
           }
           catch (Exception ex)
           {
               _logger.LogError(ex, "订单创建失败：{OrderId}", order.Id);
               throw;
           }
       }
   }

   // 日志级别：Trace < Debug < Information < Warning < Error < Critical
