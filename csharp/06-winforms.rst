====================
Windows Forms
====================

WinForms 是 .NET 最经典的桌面 UI 框架，快速开发 Windows 桌面应用。

WinForms 概述
===============

.. list-table::
   :header-rows: 1

   * - 特性
     - 说明
   * - 架构
     - 基于 GDI+ 绘图，封装 Windows 控件
   * - 设计器
     - 拖拽控件，所见即所得
   * - 事件模型
     - 基于委托的事件驱动
   * - 适用场景
     - 快速工具、内部管理系统、工业控制
   * - .NET 支持
     - .NET Framework 全系列 + .NET 6+

创建 WinForms 项目
====================

.. code-block:: text

   dotnet new winforms -n MyWinFormsApp
   cd MyWinFormsApp
   dotnet run

基本结构
---------

.. code-block:: csharp

   // Form1.cs
   public partial class Form1 : Form
   {
       public Form1()
       {
           InitializeComponent();  // 设计器生成的代码
       }
   }

   // Form1.Designer.cs（设计器自动维护）
   partial class Form1
   {
       private System.ComponentModel.IContainer components = null;
       private Button buttonOK;
       private TextBox textBoxName;
       private Label labelTitle;

       private void InitializeComponent()
       {
           this.buttonOK = new Button();
           this.textBoxName = new TextBox();
           this.labelTitle = new Label();
           // ... 属性设置
       }
   }

常用控件
=========

.. code-block:: csharp

   // 标签
   var label = new Label
   {
       Text = "用户名:",
       Location = new Point(20, 20),
       Size = new Size(80, 25),
       Font = new Font("微软雅黑", 10)
   };

   // 文本框
   var textBox = new TextBox
   {
       Location = new Point(100, 20),
       Size = new Size(200, 25),
       PasswordChar = '*'        // 密码模式
   };

   // 按钮
   var button = new Button
   {
       Text = "登录",
       Location = new Point(100, 60),
       Size = new Size(100, 35),
       BackColor = Color.LightBlue,
       FlatStyle = FlatStyle.Flat
   };
   button.Click += ButtonLogin_Click;  // 事件绑定

   // 下拉框
   var comboBox = new ComboBox
   {
       Location = new Point(100, 60),
       DropDownStyle = ComboBoxStyle.DropDownList
   };
   comboBox.Items.AddRange(["选项1", "选项2", "选项3"]);

   // 数据表格
   var dataGrid = new DataGridView
   {
       Location = new Point(20, 100),
       Size = new Size(400, 200),
       AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill,
       AllowUserToAddRows = false,
       ReadOnly = true
   };
   dataGrid.DataSource = userList;

事件处理
=========

.. code-block:: csharp

   public partial class MainForm : Form
   {
       public MainForm()
       {
           InitializeComponent();
           Load += MainForm_Load;           // 窗体加载
           FormClosing += MainForm_FormClosing;  // 关闭前
       }

       private void MainForm_Load(object? sender, EventArgs e)
       {
           // 初始化数据
           LoadUserData();
       }

       private void MainForm_FormClosing(object? sender,
           FormClosingEventArgs e)
       {
           if (HasUnsavedChanges)
           {
               var result = MessageBox.Show(
                   "有未保存的修改，确定退出？",
                   "确认",
                   MessageBoxButtons.YesNo,
                   MessageBoxIcon.Question);
               e.Cancel = (result == DialogResult.No);
           }
       }

       private void ButtonLogin_Click(object? sender, EventArgs e)
       {
           string user = textBoxUser.Text;
           string pass = textBoxPassword.Text;

           if (Authenticate(user, pass))
           {
               MessageBox.Show("登录成功！", "提示",
                   MessageBoxButtons.OK, MessageBoxIcon.Information);
           }
           else
           {
               MessageBox.Show("用户名或密码错误！", "错误",
                   MessageBoxButtons.OK, MessageBoxIcon.Error);
           }
       }
   }

对话框
========

.. code-block:: csharp

   // 消息框
   MessageBox.Show("消息内容", "标题");
   MessageBox.Show("确认删除？", "确认",
       MessageBoxButtons.YesNoCancel,
       MessageBoxIcon.Warning);

   // 打开文件
   using var openDlg = new OpenFileDialog
   {
       Filter = "文本文件 (*.txt)|*.txt|所有文件 (*.*)|*.*",
       Multiselect = false,
       InitialDirectory = Environment.GetFolderPath(
           Environment.SpecialFolder.Desktop)
   };
   if (openDlg.ShowDialog() == DialogResult.OK)
   {
       string path = openDlg.FileName;
       // 处理文件...
   }

   // 保存文件
   using var saveDlg = new SaveFileDialog
   {
       Filter = "图片文件|*.png;*.jpg",
       DefaultExt = "png"
   };
   if (saveDlg.ShowDialog() == DialogResult.OK)
   {
       image.Save(saveDlg.FileName);
   }

   // 文件夹选择
   using var folderDlg = new FolderBrowserDialog();
   if (folderDlg.ShowDialog() == DialogResult.OK)
   {
       string path = folderDlg.SelectedPath;
   }

MDI 多文档界面
================

.. code-block:: csharp

   // 主窗体设置为 MDI 容器
   public partial class MainForm : Form
   {
       public MainForm()
       {
           IsMdiContainer = true;  // 设置为 MDI 容器

           var menu = new MenuStrip();
           var fileMenu = menu.Items.Add("文件") as ToolStripMenuItem;
           fileMenu.DropDownItems.Add("打开", null, OpenChildForm);

           MainMenuStrip = menu;
           Controls.Add(menu);
       }

       private void OpenChildForm(object? sender, EventArgs e)
       {
           var child = new ChildForm();
           child.MdiParent = this;   // 设置在容器内显示
           child.Show();
       }
   }

自定义控件
============

.. code-block:: csharp

   // 继承现有控件
   public class NumericTextBox : TextBox
   {
       public NumericTextBox()
       {
           this.Font = new Font("Consolas", 12);
       }

       protected override void OnKeyPress(KeyPressEventArgs e)
       {
           // 只允许数字输入
           if (!char.IsDigit(e.KeyChar) &&
               !char.IsControl(e.KeyChar))
               e.Handled = true;

           base.OnKeyPress(e);
       }
   }

   // UserControl（组合多个控件）
   public partial class AddressControl : UserControl
   {
       public AddressControl()
       {
           InitializeComponent();  // 设计器支持
       }

       public string Street => textStreet.Text;
       public string City => textCity.Text;
       public string ZipCode => textZip.Text;
   }

.. tip::

   WinForms 开发建议：

   - 使用 ``using`` 管理 GDI 资源（Brush、Pen、Font）
   - 耗时操作使用 ``async/await`` 避免 UI 阻塞
   - 跨线程访问 UI 用 ``Invoke``/``BeginInvoke``
   - 考虑 MVP (Model-View-Presenter) 模式分离逻辑
