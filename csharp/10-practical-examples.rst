======================================
C# 编程实例
======================================

实例一：文件批量重命名工具
============================

.. code-block:: csharp
   :caption: BatchRename.cs

   using System.Text.RegularExpressions;

   public static class BatchRename
   {
       /// <summary>
       /// 批量重命名文件
       /// </summary>
       /// <param name="directory">目标目录</param>
       /// <param name="pattern">搜索模式 如 "*.txt"</param>
       /// <param name="replaceFunc">重命名函数</param>
       /// <returns>重命名的文件数量</returns>
       public static int RenameFiles(
           string directory,
           string pattern,
           Func<string, string> replaceFunc)
       {
           var files = Directory.GetFiles(directory, pattern);
           int count = 0;

           foreach (var filePath in files)
           {
               var dir = Path.GetDirectoryName(filePath)!;
               var fileName = Path.GetFileName(filePath);
               var newName = replaceFunc(fileName);

               if (newName != fileName)
               {
                   var newPath = Path.Combine(dir, newName);
                   File.Move(filePath, newPath);
                   count++;
                   Console.WriteLine($"重命名: {fileName} -> {newName}");
               }
           }

           return count;
       }
   }

   // 使用示例
   // 添加前缀："IMG_" + 原文件名
   BatchRename.RenameFiles(@"C:\Photos", "*.jpg",
       name => $"IMG_{name}");

   // 替换空格为下划线
   BatchRename.RenameFiles(@"C:\Docs", "*.pdf",
       name => name.Replace(" ", "_"));

   // 序号重命名（正则匹配）
   int seq = 1;
   BatchRename.RenameFiles(@"C:\Data", "*.csv",
       name => $"{seq++:D4}_{name}");

实例二：日志分析工具
======================

.. code-block:: csharp
   :caption: LogParser.cs

   public class LogEntry
   {
       public DateTime Timestamp { get; set; }
       public string Level { get; set; } = "";
       public string Message { get; set; } = "";
       public string? Source { get; set; }
   }

   public class LogParser
   {
       private static readonly Regex LogPattern = new(
           @"^\[(?<time>.*?)\]\s\[(?<level>\w+)\]\s(?<message>.*)$",
           RegexOptions.Compiled);

       public async IAsyncEnumerable<LogEntry> ParseAsync(
           string filePath,
           [EnumeratorCancellation] CancellationToken ct = default)
       {
           await foreach (var line in File.ReadLinesAsync(filePath, ct))
           {
               ct.ThrowIfCancellationRequested();

               var match = LogPattern.Match(line);
               if (match.Success)
               {
                   yield return new LogEntry
                   {
                       Timestamp = DateTime.Parse(match.Groups["time"].Value),
                       Level = match.Groups["level"].Value,
                       Message = match.Groups["message"].Value,
                   };
               }
           }
       }

       public async Task<LogSummary> AnalyzeAsync(
           string filePath, CancellationToken ct = default)
       {
           var summary = new LogSummary();

           await foreach (var entry in ParseAsync(filePath, ct))
           {
               summary.Total++;

               if (!summary.LevelCount.ContainsKey(entry.Level))
                   summary.LevelCount[entry.Level] = 0;
               summary.LevelCount[entry.Level]++;

               if (entry.Level == "ERROR")
                   summary.Errors.Add(entry);
           }

           return summary;
       }
   }

   public class LogSummary
   {
       public int Total { get; set; }
       public Dictionary<string, int> LevelCount { get; set; } = new();
       public List<LogEntry> Errors { get; set; } = new();

       public void PrintReport()
       {
           Console.WriteLine($"总计日志数: {Total}");
           Console.WriteLine($"错误数: {Errors.Count}");
           Console.WriteLine("\n级别统计:");

           foreach (var (level, count) in LevelCount
               .OrderByDescending(x => x.Value))
           {
               var bar = new string('#', count * 40 / Total);
               Console.WriteLine($"  {level,-12} {count,6}  {bar}");
           }
       }
   }

实例三：MVVM 待办事项应用（WPF）
===================================

Model
------

.. code-block:: csharp

   public class TodoItem
   {
       public int Id { get; set; }
       public string Title { get; set; } = "";
       public bool IsCompleted { get; set; }
       public DateTime CreatedAt { get; set; } = DateTime.Now;
       public Priority Priority { get; set; } = Priority.Medium;
   }

   public enum Priority { Low, Medium, High }

ViewModel
----------

.. code-block:: csharp

   public class TodoViewModel : ViewModelBase
   {
       private readonly ObservableCollection<TodoItem> _items = new();

       public ObservableCollection<TodoItem> Items => _items;

       private string _newItemTitle = "";
       public string NewItemTitle
       {
           get => _newItemTitle;
           set
           {
               SetProperty(ref _newItemTitle, value);
               AddCommand.RaiseCanExecuteChanged();
           }
       }

       private TodoItem? _selectedItem;
       public TodoItem? SelectedItem
       {
           get => _selectedItem;
           set => SetProperty(ref _selectedItem, value);
       }

       public IEnumerable<TodoItem> PendingItems
           => Items.Where(i => !i.IsCompleted);

       public IEnumerable<TodoItem> CompletedItems
           => Items.Where(i => i.IsCompleted);

       public RelayCommand AddCommand { get; }
       public RelayCommand<TodoItem> ToggleCommand { get; }
       public RelayCommand<TodoItem> DeleteCommand { get; }

       public TodoViewModel()
       {
           AddCommand = new RelayCommand(
               execute: _ =>
               {
                   Items.Add(new TodoItem
                   {
                       Id = Items.Count + 1,
                       Title = NewItemTitle,
                       Priority = Priority.Medium
                   });
                   NewItemTitle = "";
               },
               canExecute: _ => !string.IsNullOrWhiteSpace(NewItemTitle));

           ToggleCommand = new RelayCommand<TodoItem>(
               execute: item =>
               {
                   item.IsCompleted = !item.IsCompleted;
                   OnPropertyChanged(nameof(PendingItems));
                   OnPropertyChanged(nameof(CompletedItems));
               });

           DeleteCommand = new RelayCommand<TodoItem>(
               execute: item =>
               {
                   Items.Remove(item);
               });
       }
   }

View（MainWindow.xaml）
------------------------

.. code-block:: xml

   <Window x:Class="TodoApp.MainWindow"
           Title="待办事项" Height="450" Width="500">
       <Grid Margin="12">
           <Grid.RowDefinitions>
               <RowDefinition Height="Auto"/>
               <RowDefinition Height="*"/>
               <RowDefinition Height="Auto"/>
           </Grid.RowDefinitions>

           <!-- 添加新任务 -->
           <StackPanel Orientation="Horizontal" Grid.Row="0"
                       Margin="0,0,0,12">
               <TextBox Text="{Binding NewItemTitle}"
                        Width="350" PlaceholderText="输入新任务..."/>
               <Button Content="添加" Command="{Binding AddCommand}"
                       Width="80" Margin="10,0,0,0"/>
           </StackPanel>

           <!-- 任务列表 -->
           <ListBox Grid.Row="1"
                    ItemsSource="{Binding Items}"
                    SelectedItem="{Binding SelectedItem}">
               <ListBox.ItemTemplate>
                   <DataTemplate>
                       <StackPanel Orientation="Horizontal">
                           <CheckBox IsChecked="{Binding IsCompleted}"
                                     Command="{Binding
                                 DataContext.ToggleCommand,
                                 RelativeSource={RelativeSource
                                 AncestorType=Window}}"
                                     CommandParameter="{Binding}"/>
                           <TextBlock Text="{Binding Title}"
                                      FontSize="14"
                                      TextDecorations="{Binding IsCompleted,
                                          Converter={StaticResource StrikeConverter}}"
                                      Margin="8,0"/>
                           <TextBlock Text="{Binding Priority}"
                                      Foreground="Gray" FontSize="12"/>
                       </StackPanel>
                   </DataTemplate>
               </ListBox.ItemTemplate>
           </ListBox>

           <!-- 统计信息 -->
           <TextBlock Grid.Row="2" Margin="0,12,0,0">
               <Run Text="总计:"/><Run Text="{Binding Items.Count}"/>
               <Run Text=" 已完成:"/>
               <Run Text="{Binding CompletedItems.Count}"/>
               <Run Text=" 待完成:"/>
               <Run Text="{Binding PendingItems.Count}"/>
           </TextBlock>
       </Grid>
   </Window>
