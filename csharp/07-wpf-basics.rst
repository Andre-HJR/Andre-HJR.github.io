======================================
WPF 基础
======================================

WPF（Windows Presentation Foundation）是微软新一代的桌面 UI 框架，
采用 XAML + 数据驱动的设计理念。

WPF 概述
==========

.. list-table::
   :header-rows: 1

   * - 特性
     - WinForms
     - WPF
   * - 渲染引擎
     - GDI+（像素级）
     - DirectX（硬件加速）
   * - UI 定义
     - 代码（设计器生成）
     - XAML（声明式）
   * - 布局模型
     - 绝对定位
     - 流式布局（Panel/Grid）
   * - 数据绑定
     - 手动
     - 内置 + 双向绑定
   * - 样式
     - 逐控件设置
     - 样式/模板/触发器
   * - 可定制性
     - 有限
     - 任意控件可完全重模板
   * - 学习曲线
     - 平缓
     - 较陡（MVVM + 模板）

XAML 基础
=============

.. code-block:: xml
   :caption: MainWindow.xaml

   <Window x:Class="WpfApp.MainWindow"
           xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
           xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
           Title="WPF Demo" Height="450" Width="800">

       <Grid>
           <!-- 定义行和列 -->
           <Grid.RowDefinitions>
               <RowDefinition Height="Auto"/>
               <RowDefinition Height="*"/>
               <RowDefinition Height="Auto"/>
           </Grid.RowDefinitions>

           <!-- 工具栏 -->
           <ToolBar Grid.Row="0">
               <Button Content="打开"/>
               <Button Content="保存"/>
           </ToolBar>

           <!-- 内容区域 -->
           <ListBox Grid.Row="1"
                    ItemsSource="{Binding Users}"
                    SelectedItem="{Binding SelectedUser}"
                    DisplayMemberPath="Name"/>

           <!-- 状态栏 -->
           <StatusBar Grid.Row="2">
               <TextBlock Text="{Binding StatusText}"/>
           </StatusBar>
       </Grid>
   </Window>

布局系统
=========

.. code-block:: xml

   <!-- Grid：最灵活的布局容器 -->
   <Grid>
       <Grid.ColumnDefinitions>
           <ColumnDefinition Width="Auto"/>   <!-- 自适应 -->
           <ColumnDefinition Width="*"/>      <!-- 剩余空间 -->
           <ColumnDefinition Width="2*"/>     <!-- 比例 -->
       </Grid.ColumnDefinitions>
       <Grid.RowDefinitions>
           <RowDefinition Height="30"/>
           <RowDefinition Height="*"/>
       </Grid.RowDefinitions>

       <TextBlock Grid.Row="0" Grid.Column="0" Text="名称:"/>
       <TextBox  Grid.Row="0" Grid.Column="1" Text="{Binding Name}"/>
   </Grid>

   <!-- StackPanel：水平/垂直堆叠 -->
   <StackPanel Orientation="Horizontal">
       <Button Content="确定" Margin="5"/>
       <Button Content="取消" Margin="5"/>
   </StackPanel>

   <!-- WrapPanel：自动换行 -->
   <WrapPanel>
       <Button Content="标签1"/>
       <Button Content="标签2"/>
       <!-- 空间不足自动换行 -->
   </WrapPanel>

   <!-- DockPanel：停靠布局 -->
   <DockPanel>
       <Menu DockPanel.Dock="Top">...</Menu>
       <StatusBar DockPanel.Dock="Bottom">...</StatusBar>
       <TextBox/>   <!-- 填充剩余空间 -->
   </DockPanel>

数据绑定
=========

核心概念
----------

.. code-block::

   绑定模式：
   OneWay      ：源 → 目标（默认）
   TwoWay      ：源 ↔ 目标
   OneTime     ：初始化时一次
   OneWayToSource：目标 → 源

   绑定方向：Source （数据源）→ Target （UI 控件）

XAML 绑定语法
---------------

.. code-block:: xml

   <!-- 基本绑定 -->
   <TextBox Text="{Binding UserName}"/>
   <TextBlock Text="{Binding FullName, FallbackValue='N/A'}"/>

   <!-- 格式化 -->
   <TextBlock Text="{Binding Price, StringFormat={}￥{0:N2}}"/>
   <TextBlock Text="{Binding Date, StringFormat={}{0:yyyy-MM-dd}}"/>

   <!-- 转换器 -->
   <TextBlock Text="{Binding Status, Converter={StaticResource StatusConverter}}"/>

   <!-- 相对源 -->
   <TextBlock Text="{Binding RelativeSource={RelativeSource AncestorType=Window},
                             Path=Title}"/>

   <!-- 通知属性变更 -->
   <CheckBox IsChecked="{Binding IsActive, Mode=TwoWay}"/>

INotifyPropertyChanged
-----------------------

.. code-block:: csharp

   public class ViewModelBase : INotifyPropertyChanged
   {
       public event PropertyChangedEventHandler? PropertyChanged;

       protected void SetProperty<T>(ref T field, T value,
           [CallerMemberName] string? propertyName = null)
       {
           if (!EqualityComparer<T>.Default.Equals(field, value))
           {
               field = value;
               PropertyChanged?.Invoke(this,
                   new PropertyChangedEventArgs(propertyName));
           }
       }
   }

   public class MainViewModel : ViewModelBase
   {
       private string _name = "";

       public string Name
       {
           get => _name;
           set => SetProperty(ref _name, value);
       }
   }

命令（ICommand）
==================

.. code-block:: csharp

   // RelayCommand 实现
   public class RelayCommand : ICommand
   {
       private readonly Action<object?> _execute;
       private readonly Func<object?, bool>? _canExecute;

       public RelayCommand(Action<object?> execute,
                          Func<object?, bool>? canExecute = null)
       {
           _execute = execute;
           _canExecute = canExecute;
       }

       public bool CanExecute(object? parameter)
           => _canExecute?.Invoke(parameter) ?? true;

       public void Execute(object? parameter)
           => _execute(parameter);

       public event EventHandler? CanExecuteChanged
       {
           add => CommandManager.RequerySuggested += value;
           remove => CommandManager.RequerySuggested -= value;
       }
   }

.. code-block:: xml

   <Button Content="保存"
           Command="{Binding SaveCommand}"
           CommandParameter="{Binding Id}"/>

.. code-block:: csharp

   // ViewModel 中的命令
   public ICommand SaveCommand { get; }

   public MainViewModel()
   {
       SaveCommand = new RelayCommand(
           execute: param => SaveData(param),
           canExecute: _ => !IsBusy);
   }

样式与资源
============

.. code-block:: xml

   <!-- 资源字典 -->
   <Window.Resources>
       <!-- 样式 -->
       <Style TargetType="Button" x:Key="PrimaryButton">
           <Setter Property="Background" Value="#0078D4"/>
           <Setter Property="Foreground" Value="White"/>
           <Setter Property="Padding" Value="12,6"/>
           <Setter Property="FontSize" Value="14"/>
           <Setter Property="Template">
               <Setter.Value>
                   <ControlTemplate TargetType="Button">
                       <Border Background="{TemplateBinding Background}"
                               CornerRadius="4">
                           <ContentPresenter HorizontalAlignment="Center"
                                             VerticalAlignment="Center"/>
                       </Border>
                   </ControlTemplate>
               </Setter.Value>
           </Setter>
       </Style>

       <!-- 隐式样式（自动应用到所有 Button） -->
       <Style TargetType="Button" BasedOn="{StaticResource PrimaryButton}"/>

       <!-- 数据模板 -->
       <DataTemplate x:Key="UserTemplate">
           <StackPanel Orientation="Horizontal">
               <TextBlock Text="{Binding Name}" FontWeight="Bold"/>
               <TextBlock Text="{Binding Email}" Margin="10,0,0,0"/>
           </StackPanel>
       </DataTemplate>
   </Window.Resources>
