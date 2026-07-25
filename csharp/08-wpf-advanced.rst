======================================
WPF 进阶：MVVM 与实践
======================================

MVVM 模式是 WPF 的核心设计模式。

MVVM 模式详解
===============

.. code-block::

   ┌─────────────────────────────────────────────────────────┐
   │  View（XAML）                                            │
   │  负责 UI 呈现、动画、视觉布局                              │
   │  通过 Binding 连接到 ViewModel                           │
   │  × 尽量不包含代码逻辑                                     │
   ├─────────────────────────────────────────────────────────┤
   │  ViewModel                                               │
   │  负责 UI 状态和逻辑                                       │
   │  实现 INotifyPropertyChanged                             │
   │  暴露属性 + 命令（ICommand）                               │
   │  × 不直接引用 UI 控件                                     │
   ├─────────────────────────────────────────────────────────┤
   │  Model                                                   │
   │  业务数据 + 领域逻辑                                      │
   │  数据库/网络/文件访问                                      │
   │  × 不知道 View 和 ViewModel 的存在                         │
   └─────────────────────────────────────────────────────────┘

完整 MVVM 示例
=================

Model
------

.. code-block:: csharp

   public class UserModel
   {
       public int Id { get; set; }
       public string Name { get; set; } = "";
       public string Email { get; set; } = "";
       public DateTime CreatedAt { get; set; }
   }

ViewModel
----------

.. code-block:: csharp

   public class UserViewModel : ViewModelBase
   {
       private readonly IUserService _userService;
       private ObservableCollection<UserModel> _users = new();

       public ObservableCollection<UserModel> Users
       {
           get => _users;
           set => SetProperty(ref _users, value);
       }

       private UserModel? _selectedUser;
       public UserModel? SelectedUser
       {
           get => _selectedUser;
           set
           {
               SetProperty(ref _selectedUser, value);
               DeleteCommand.RaiseCanExecuteChanged();
           }
       }

       private bool _isLoading;
       public bool IsLoading
       {
           get => _isLoading;
           set => SetProperty(ref _isLoading, value);
       }

       private string _searchText = "";
       public string SearchText
       {
           get => _searchText;
           set
           {
               SetProperty(ref _searchText, value);
               _ = SearchAsync();  // 输入即搜索
           }
       }

       // 命令
       public AsyncRelayCommand LoadCommand { get; }
       public RelayCommand DeleteCommand { get; }

       public UserViewModel(IUserService userService)
       {
           _userService = userService;

           LoadCommand = new AsyncRelayCommand(async () =>
           {
               IsLoading = true;
               try
               {
                   var result = await _userService.GetAllAsync();
                   Users = new ObservableCollection<UserModel>(result);
               }
               finally
               {
                   IsLoading = false;
               }
           });

           DeleteCommand = new RelayCommand(
               execute: async _ =>
               {
                   if (SelectedUser == null) return;
                   await _userService.DeleteAsync(SelectedUser.Id);
                   Users.Remove(SelectedUser);
               },
               canExecute: _ => SelectedUser != null);
       }

       private async Task SearchAsync()
       {
           if (string.IsNullOrWhiteSpace(SearchText))
           {
               await LoadCommand.ExecuteAsync(null);
               return;
           }

           var result = await _userService.SearchAsync(SearchText);
           Users = new ObservableCollection<UserModel>(result);
       }
   }

View（XAML）
-------------

.. code-block:: xml

   <Window x:Class="WpfApp.Views.UserListView"
           xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
           xmlns:i="http://schemas.microsoft.com/xaml/behaviors"
           Title="用户管理" Height="600" Width="900">

       <Grid Margin="12">
           <Grid.RowDefinitions>
               <RowDefinition Height="Auto"/>
               <RowDefinition Height="*"/>
               <RowDefinition Height="Auto"/>
           </Grid.RowDefinitions>

           <!-- 搜索栏 -->
           <StackPanel Orientation="Horizontal" Grid.Row="0" Margin="0,0,0,12">
               <TextBox Text="{Binding SearchText, UpdateSourceTrigger=PropertyChanged}"
                        Width="250" PlaceholderText="搜索用户..."/>
               <Button Content="加载" Command="{Binding LoadCommand}"
                       Width="80" Margin="10,0,0,0"/>
           </StackPanel>

           <!-- 用户列表 -->
           <DataGrid Grid.Row="1"
                     ItemsSource="{Binding Users}"
                     SelectedItem="{Binding SelectedUser}"
                     AutoGenerateColumns="False"
                     IsReadOnly="True"
                     AlternatingRowBackground="#F5F5F5">
               <DataGrid.Columns>
                   <DataGridTextColumn Header="ID" Binding="{Binding Id}" Width="50"/>
                   <DataGridTextColumn Header="姓名" Binding="{Binding Name}" Width="150"/>
                   <DataGridTextColumn Header="邮箱" Binding="{Binding Email}" Width="*"/>
                   <DataGridTextColumn Header="创建时间"
                       Binding="{Binding CreatedAt, StringFormat={}{0:yyyy-MM-dd}}"
                       Width="120"/>
               </DataGrid.Columns>
           </DataGrid>

           <!-- 状态栏 -->
           <StatusBar Grid.Row="2" Margin="0,12,0,0">
               <TextBlock Text="{Binding StatusText}"/>
               <Separator/>
               <TextBlock Text="{Binding Users.Count, StringFormat=共 {0} 条记录}"/>
           </StatusBar>
       </Grid>
   </Window>

Code-Behind（仅 View 层）
--------------------------

.. code-block:: csharp

   public partial class UserListView : Window
   {
       public UserListView()
       {
           InitializeComponent();

           // ViewModel 通过 DI 注入
           DataContext = App.GetService<UserViewModel>();

           Loaded += async (_, _) =>
           {
               if (DataContext is UserViewModel vm)
                   await vm.LoadCommand.ExecuteAsync(null);
           };
       }
   }

依赖注入集成
==============

.. code-block:: csharp

   // App.xaml.cs
   public partial class App : Application
   {
       private static IServiceProvider? _serviceProvider;

       protected override void OnStartup(StartupEventArgs e)
       {
           var services = new ServiceCollection();

           // 注册服务
           services.AddSingleton<IUserService, UserService>();
           services.AddDbContext<AppDbContext>();

           // 注册 ViewModels
           services.AddTransient<UserViewModel>();
           services.AddTransient<MainViewModel>();

           // 注册 Views
           services.AddTransient<UserListView>();
           services.AddTransient<MainWindow>();

           _serviceProvider = services.BuildServiceProvider();

           // 启动主窗口
           var mainWindow = _serviceProvider.GetRequiredService<MainWindow>();
           mainWindow.Show();
       }

       public static T GetService<T>() where T : notnull
           => _serviceProvider!.GetRequiredService<T>();
   }

触发器与动画
==============

.. code-block:: xml

   <Button Content="悬停效果">
       <Button.Style>
           <Style TargetType="Button">
               <Setter Property="Background" Value="Gray"/>

               <!-- 属性触发器 -->
               <Style.Triggers>
                   <Trigger Property="IsMouseOver" Value="True">
                       <Setter Property="Background" Value="Blue"/>
                       <Setter Property="Foreground" Value="White"/>
                   </Trigger>
                   <Trigger Property="IsPressed" Value="True">
                       <Setter Property="Background" Value="DarkBlue"/>
                   </Trigger>
               </Style.Triggers>
           </Style>
       </Button.Style>
   </Button>

   <!-- DataTrigger（根据数据改变 UI） -->
   <TextBlock Text="{Binding Status}">
       <TextBlock.Style>
           <Style TargetType="TextBlock">
               <Style.Triggers>
                   <DataTrigger Binding="{Binding Status}" Value="Error">
                       <Setter Property="Foreground" Value="Red"/>
                       <Setter Property="FontWeight" Value="Bold"/>
                   </DataTrigger>
                   <DataTrigger Binding="{Binding Status}" Value="Success">
                       <Setter Property="Foreground" Value="Green"/>
                   </DataTrigger>
               </Style.Triggers>
           </Style>
       </TextBlock.Style>
   </TextBlock>

转换器
========

.. code-block:: csharp

   // 值转换器
   [ValueConversion(typeof(bool), typeof(Visibility))]
   public class BoolToVisibilityConverter : IValueConverter
   {
       public object Convert(object value, Type targetType,
           object parameter, CultureInfo culture)
       {
           bool boolValue = (bool)value;
           return boolValue ? Visibility.Visible : Visibility.Collapsed;
       }

       public object ConvertBack(object value, Type targetType,
           object parameter, CultureInfo culture)
       {
           return (Visibility)value == Visibility.Visible;
       }
   }

   // 注册并使用
   // <Window.Resources>
   //   <local:BoolToVisibilityConverter x:Key="BoolToVis"/>
   // </Window.Resources>
   // <TextBlock Visibility="{Binding IsActive,
   //                   Converter={StaticResource BoolToVis}}"/>
