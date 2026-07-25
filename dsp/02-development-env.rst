================================
CCS 开发环境搭建
================================

Code Composer Studio (CCS) 是 TI 官方 IDE，基于 Eclipse。

安装 CCS
=========

.. code-block::

   1. 下载 CCS:
      https://www.ti.com/tool/CCSTUDIO

   2. 安装时选择组件:
      - C2000 系列支持（必选）
      - XDS100/XDS110 调试器驱动
      - CGT 编译器工具链

   3. 安装完成后启动 CCS

   4. 首选项设置:
      Window → Preferences → Code Composer Studio → Products
      确认 C2000 Compiler 已识别

项目结构
=========

.. code-block::

   my_project/
   ├── .project / .cproject          # CCS 项目配置
   ├── targetConfigs/                # 目标配置文件
   │   └── TMS320F28335.ccxml        # 芯片 + 调试器配置
   ├── device/                       # 设备支持文件
   │   ├── DSP2833x_Headers_nonBIOS.cmd  # 外设寄存器链接文件
   │   └── F28335.cmd                    # 存储器分配链接文件
   ├── source/                       # 源码目录
   │   ├── main.c
   │   ├── DSP2833x_GlobalVariableDefs.c
   │   ├── DSP2833x_Startup.c
   │   ├── DSP2833x_Sci.c
   │   └── DSP2833x_PieVect.c
   └── include/                      # 头文件
       └── DSP2833x_Device.h

.. tip::

   建议从 TI C2000Ware 中的例程开始修改，
   而不是从零搭建项目。

C2000Ware SDK
===============

.. code-block::

   # 下载 C2000Ware
   https://www.ti.com/tool/C2000WARE

   # 目录结构
   C2000Ware_x_xx_xx_xx/
   ├── device_support/       # 设备支持包
   │   └── f2833x/
   │       ├── examples/     # 官方例程
   │       ├── headers/      # 寄存器头文件
   │       └── cmd/          # 链接文件
   ├── driverlib/            # 驱动库 (较新型号)
   └── libraries/            # DSP 库 (IQMath, Filter)

.. important::

   F28335 等较老型号主要使用**寄存器级别**的头文件（DSP2833x_Header），
   而 F28004x/F2838x 等新型号建议使用 **driverlib** 驱动库。

创建第一个项目
===============

.. code-block::

   方法一：导入例程
   1. Project → Import CCS Projects
   2. 选择 C2000Ware 中的例程路径
   3. 例如: C2000Ware/device_support/f2833x/examples/led_blink
   4. 点击 Finish

   方法二：新建项目
   1. Project → New CCS Project
   2. Target: TMS320F28335
   3. Connection: Texas Instruments XDS100v2 USB Emulator
   4. Project name: my_project
   5. 选择 Empty Project (with main.c)
   6. 点击 Finish

.. code-block:: c
   :caption: main.c (LED 闪烁)

   #include "DSP2833x_Device.h"     // 设备头文件
   #include "DSP2833x_Examples.h"   // 例程共用定义

   void main(void)
   {
       // 系统初始化
       InitSysCtrl();                // 系统时钟/PLL/看门狗
       InitPieCtrl();                // PIE 中断初始化
       InitPieVectTable();           // 中断向量表

       // 配置 GPIO 为输出
       EALLOW;
       GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;  // GPIO 模式
       GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;   // 输出
       EDIS;

       while (1)
       {
           GpioDataRegs.GPADAT.bit.GPIO0 = 1;  // 亮
           DELAY_US(500000);                    // 500ms
           GpioDataRegs.GPADAT.bit.GPIO0 = 0;  // 灭
           DELAY_US(500000);
       }
   }

编译与烧录
===========

.. code-block::

   编译:     Project → Build Project   (Ctrl+B)
   烧录:     Run → Debug                (F11)
   运行:     Run → Resume               (F8)
   暂停:     Run → Suspend              (Alt+F8)
   停止:     Run → Terminate            (Ctrl+F2)
   断点:     双击行号左侧                (Ctrl+Shift+B)

   调试视图:
   - Expressions: 添加变量观察
   - Registers:   查看 CPU 和外设寄存器
   - Memory       Browser: 直接查看/修改内存
   - Disassembly: 反汇编窗口

.. code-block::

   Debug 模式下会：
   1. 连接仿真器 (XDS100)
   2. 初始化 JTAG
   3. 下载 .out 文件到 Flash/RAM
   4. 跳转到 main() 入口
   5. 等待用户点击 Resume

.. tip::

   C2000 开发调试技巧：

   - **RAM vs Flash**：调试时加载到 RAM（烧录快），最终发布烧录到 Flash
   - **实时模式**：Run → Realtime Options → Enable Realtime Mode
     可在 DSP 运行时修改寄存器
   - **Graph**：Tools → Graph → Dual Time/Frequency
     实时观察 ADC 采样波形
   - **断点类型**：硬件断点（不限数量）vs 软件断点（仅 Flash 中）
