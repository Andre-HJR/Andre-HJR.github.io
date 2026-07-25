======================================
数据访问
======================================

.NET 提供多种数据访问技术，从 ADO.NET 到 Entity Framework Core。

ADO.NET
=========

ADO.NET 是 .NET 最底层的数据访问 API，提供直接执行 SQL 的能力。

.. code-block:: csharp

   using System.Data;
   using System.Data.SqlClient;

   // 连接字符串
   string connStr = "Server=.;Database=MyDb;Trusted_Connection=true;TrustServerCertificate=true;";

   // 查询
   using var conn = new SqlConnection(connStr);
   await conn.OpenAsync();

   var cmd = new SqlCommand("SELECT Id, Name, Email FROM Users WHERE Id = @id", conn);
   cmd.Parameters.AddWithValue("@id", userId);

   using var reader = await cmd.ExecuteReaderAsync();
   while (await reader.ReadAsync())
   {
       int id = reader.GetInt32(0);
       string name = reader.GetString(1);
       string? email = reader.IsDBNull(2) ? null : reader.GetString(2);
   }

   // 非查询操作（INSERT / UPDATE / DELETE）
   var insertCmd = new SqlCommand(@"
       INSERT INTO Users (Name, Email, CreatedAt)
       VALUES (@name, @email, @createdAt)", conn);

   insertCmd.Parameters.AddWithValue("@name", "Alice");
   insertCmd.Parameters.AddWithValue("@email", "alice@example.com");
   insertCmd.Parameters.AddWithValue("@createdAt", DateTime.UtcNow);

   int rows = await insertCmd.ExecuteNonQueryAsync();

   // 事务
   using var tx = await conn.BeginTransactionAsync();
   try
   {
       cmd.Transaction = (SqlTransaction)tx;
       await cmd.ExecuteNonQueryAsync();
       await tx.CommitAsync();
   }
   catch
   {
       await tx.RollbackAsync();
       throw;
   }

.. warning::

   **始终使用参数化查询**，不要拼接 SQL 字符串，防止 SQL 注入。

   .. code-block:: csharp

      // 错误！SQL 注入风险
      var sql = $"SELECT * FROM Users WHERE Name = '{userInput}'";

      // 正确！参数化查询
      var cmd = new SqlCommand("SELECT * FROM Users WHERE Name = @name", conn);
      cmd.Parameters.AddWithValue("@name", userInput);

Entity Framework Core
=======================

EF Core 是 .NET 的首选 ORM，支持多种数据库。

DbContext 与实体
------------------

.. code-block:: csharp

   // 实体类
   public class User
   {
       public int Id { get; set; }
       public string Name { get; set; } = "";
       public string? Email { get; set; }
       public DateTime CreatedAt { get; set; }

       // 导航属性
       public ICollection<Order> Orders { get; set; } = new List<Order>();
   }

   public class Order
   {
       public int Id { get; set; }
       public int UserId { get; set; }
       public decimal Amount { get; set; }
       public DateTime OrderDate { get; set; }

       // 导航属性
       public User User { get; set; } = null!;
   }

   // DbContext
   public class AppDbContext : DbContext
   {
       public DbSet<User> Users => Set<User>();
       public DbSet<Order> Orders => Set<Order>();

       protected override void OnConfiguring(DbContextOptionsBuilder options)
       {
           options.UseSqlServer(@"Server=.;Database=MyDb;...");
       }

       protected override void OnModelCreating(ModelBuilder modelBuilder)
       {
           // Fluent API 配置
           modelBuilder.Entity<User>(entity =>
           {
               entity.HasKey(e => e.Id);
               entity.Property(e => e.Name)
                     .IsRequired()
                     .HasMaxLength(100);
               entity.HasIndex(e => e.Email).IsUnique();
           });
       }
   }

CRUD 操作
-----------

.. code-block:: csharp

   // 创建
   using var db = new AppDbContext();
   var user = new User { Name = "Alice", Email = "alice@example.com" };
   db.Users.Add(user);
   await db.SaveChangesAsync();     // Id 自动填充

   // 查询
   var user = await db.Users
       .Include(u => u.Orders)      // Eager Loading
       .FirstOrDefaultAsync(u => u.Id == id);

   var users = await db.Users
       .Where(u => u.Name.Contains("A"))
       .OrderBy(u => u.Name)
       .Skip(page * size)
       .Take(size)
       .ToListAsync();

   // 更新
   var user = await db.Users.FindAsync(id);
   if (user != null)
   {
       user.Name = "Updated Name";
       await db.SaveChangesAsync();
   }

   // 删除
   var user = await db.Users.FindAsync(id);
   if (user != null)
   {
       db.Users.Remove(user);
       await db.SaveChangesAsync();
   }

   // 原生 SQL
   var users = await db.Users
       .FromSql($"SELECT * FROM Users WHERE Name LIKE {search}")
       .ToListAsync();

Migrations
-----------

.. code-block:: text

   dotnet ef migrations add InitialCreate
   dotnet ef database update

   # 生成 SQL 脚本
   dotnet ef migrations script

Dapper（轻量级 ORM）
======================

.. code-block:: csharp

   using Dapper;

   // 需要安装 Dapper NuGet 包

   using var conn = new SqlConnection(connStr);

   // 查询
   var user = await conn.QueryFirstOrDefaultAsync<User>(
       "SELECT * FROM Users WHERE Id = @Id", new { Id = 1 });

   var users = await conn.QueryAsync<User>(
       "SELECT * FROM Users WHERE Name LIKE @Search",
       new { Search = $"%{keyword}%" });

   // 执行
   int rows = await conn.ExecuteAsync(
       "UPDATE Users SET Name = @Name WHERE Id = @Id",
       new { Name = "Alice", Id = 1 });

   // 事务
   using var tx = conn.BeginTransaction();
   await conn.ExecuteAsync("DELETE FROM Orders WHERE UserId = @Id", new { id }, tx);
   await conn.ExecuteAsync("DELETE FROM Users WHERE Id = @Id", new { id }, tx);
   tx.Commit();

.. tip::

   **EF Core vs Dapper 选择：**

   - **EF Core**：CRUD 密集、复杂关系映射、快速开发 → 选 EF Core
   - **Dapper**：性能敏感、纯 SQL 掌控、简单数据映射 → 选 Dapper
   - **兼顾**：EF Core 负责复杂业务，Dapper 负责高并发查询

文件数据库（SQLite）
=====================

.. code-block:: csharp

   // 安装 Microsoft.EntityFrameworkCore.Sqlite

   public class LocalDbContext : DbContext
   {
       public DbSet<ConfigItem> Config => Set<ConfigItem>();

       protected override void OnConfiguring(DbContextOptionsBuilder options)
       {
           options.UseSqlite("Data Source=app.db");
       }
   }

   // SQLite 适合单机桌面应用、不需要安装数据库服务
