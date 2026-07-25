================================
Flash 编程与启动引导
================================

F28335 内置 512 KB Flash，用于存储程序代码。调试时通常在 RAM 中运行，
最终产品需烧写到 Flash。

Flash 与 RAM 的区别
====================

.. list-table::
   :header-rows: 1

   * - 特性
     - RAM 调试
     - Flash 运行
   * - 加载速度
     - 快（< 1s）
     - 慢（~5~10s）
   * - 掉电保持
     - 丢失
     - 保持
   * - 运行速度
     - 150 MHz
     - 150 MHz（需配置等待状态）
   * - 代码修改
     - 可直接修改
     - 需擦除再写
   * - 适用场景
     - 开发调试
     - 最终产品

Flash 配置
===========

.. code-block:: c

   void InitFlash(void)
   {
       EALLOW;

       // 使能 Flash 流水线
       FlashRegs.FOPT.bit.ENPIPE = 1;

       // 配置等待状态
       // 150 MHz 运行时：
       FlashRegs.FBANKWAIT.bit.RANDWAIT = 5;   // 随机访问等待
       FlashRegs.FBANKWAIT.bit.PAGEWAIT = 5;   // 页访问等待
       FlashRegs.FSTATUS.bit.V3VSTAT = 1;     // 3.3V 操作

       // 配置 OTP 等待
       FlashRegs.FOTPWAIT.bit.OTPWAIT = 5;

       EDIS;

       // 等待 Flash 就绪
       while (FlashRegs.FPWR.bit.PWRSTAT == 0) { }
   }

.. warning::

   Flash 等待状态设置不足会导致程序**跑飞**。
   150 MHz 运行时至少需要 5 个等待周期（RANDWAIT = 5）。

链接文件（CMD）
=================

.. code-block::
   :caption: F28335.cmd（存储器分配）

   MEMORY
   {
       PAGE 0:                 /* 程序空间 */
       PRAMH0      : origin = 0x008000, length = 0x008000   /* 32K RAM */

       FLASH_A     : origin = 0x300000, length = 0x00FFFF   /* 64K */
       FLASH_B     : origin = 0x310000, length = 0x00FFFF   /* 64K */
       FLASH_C     : origin = 0x320000, length = 0x00FFFF   /* 64K */
       FLASH_D     : origin = 0x330000, length = 0x00FFFF   /* 64K */
       FLASH_E     : origin = 0x380000, length = 0x00FFFF   /* 64K */
       FLASH_F     : origin = 0x390000, length = 0x00FFFF   /* 64K */
       FLASH_G     : origin = 0x3A0000, length = 0x00FFFF   /* 64K */
       FLASH_H     : origin = 0x3B0000, length = 0x00FFFF   /* 64K */

       PAGE 1:                 /* 数据空间 */
       DRAMH0      : origin = 0x00C000, length = 0x008000   /* 32K RAM */
   }

   SECTIONS
   {
       /* Flash 中的代码段 */
       .text        : > FLASH_A,     PAGE = 0   /* 代码 */
       .cinit       : > FLASH_A,     PAGE = 0   /* 全局变量初始化 */
       .pinit       : > FLASH_A,     PAGE = 0   /* 构造函数 */

       /* RAM 中的数据段 */
       .ebss        : > DRAMH0,      PAGE = 1   /* 全局变量 */
       .esysmem     : > DRAMH0,      PAGE = 1   /* 堆 */
       .stack       : > DRAMH0,      PAGE = 1   /* 栈 */

       /* 将关键函数复制到 RAM 运行（提高速度） */
       ramfuncs     : LOAD = FLASH_A,           /* 存储在 Flash */
                      RUN = PRAMH0,             /* 在 RAM 中运行 */
                      LOAD_START(_RamfuncsLoadStart),
                      LOAD_END(_RamfuncsLoadEnd),
                      RUN_START(_RamfuncsRunStart),
                      PAGE = 0
   }

RAM 中运行关键函数
====================

.. code-block:: c

   // 将关键函数指定到 ramfuncs 段（在链接文件中配置）

   #pragma CODE_SECTION(interrupt_loop, "ramfuncs");
   void interrupt_loop(void)   // 电流环——在 RAM 中执行
   {
       // 高速控制代码
       // 比 Flash 运行快 2~3 倍
   }

   // 复制代码到 RAM
   void MemCopy(uint16_t *dest, uint16_t *src, uint16_t size)
   {
       while (size--) *dest++ = *src++;
   }

   void main(void)
   {
       // 将 ramfuncs 从 Flash 复制到 RAM
       MemCopy(&RamfuncsRunStart, &RamfuncsLoadStart,
               &RamfuncsLoadEnd - &RamfuncsLoadStart);

       // ... 其他初始化
   }

Boot ROM 与启动模式
=====================

.. code-block::

   F28335 上电启动流程：

   1. CPU 从 Boot ROM (0x3F FFC0) 开始执行
   2. Boot ROM 检测 GPIO 引脚状态
   3. 根据引脚电平选择启动模式

   启动模式选择（GPIO87, GPIO86, GPIO85）：

   +--------+--------+--------+------------------+
   | GPIO87 | GPIO86 | GPIO85 | 启动模式         |
   +========+========+========+==================+
   | 1      | 1      | 1      | Flash 启动       |
   +--------+--------+--------+------------------+
   | 0      | 1      | 1      | SCI 启动         |
   +--------+--------+--------+------------------+
   | 1      | 0      | 1      | SPI 启动         |
   +--------+--------+--------+------------------+
   | 1      | 1      | 0      | I2C 启动         |
   +--------+--------+--------+------------------+
   | 0      | 1      | 0      | eCAN 启动        |
   +--------+--------+--------+------------------+
   | 0      | 0      | 1      | 并行 I/O 启动     |
   +--------+--------+--------+------------------+
   | 0      | 0      | 0      | 分支到 SARAM     |
   +--------+--------+--------+------------------+

   Flash 启动是最常用的模式（GPIO87/86/85 全部上拉 = 3.3V）。

SCI 串口 Bootloader
====================

.. code-block::

   SCI Bootloader (GPIO85=1, GPIO86=1, GPIO87=0)：

   主机通过 SCI-A (GPIO28/29) 发送固件：

   1. Boot ROM 等待接收 8 字节的"魔术字"
      0x08 0xAA 0x02 0x00 0x00 0x00 0xAA 0x02

   2. 接收到后发送确认，开始接收 S19 格式的固件

   3. 烧写完成后跳转到 Flash 执行

   用途：产品现场升级（不需仿真器）

Flash API
==========

.. code-block:: c

   // 擦除 Flash 扇区
   void Flash_Erase(uint16_t sector)
   {
       EALLOW;
       FlashRegs.FERASECR = FERASE_SECTOR_BITS(sector);
       FlashRegs.FSTDBYWAIT = 0x01FF;      // 进入待机
       FlashRegs.FERASECR.bit.FRES = 0xA;  // 擦除命令
       while (FlashRegs.FSTATUS.bit.BUSY) { }
       EDIS;
   }

   // 编程 Flash（写入 64 位数据）
   void Flash_Program(uint32_t addr, uint64_t *data)
   {
       EALLOW;
       FlashRegs.FPROGCR = FPROG_SECTOR_BITS;
       FlashRegs.FSTDBYWAIT = 0x01FF;
       FlashRegs.FPROGCR.bit.FRES = 0x4;   // 编程命令

       // 写入数据到编程缓冲区
       FlashRegs.FDATA[0] = *((uint16_t *)data + 0);
       FlashRegs.FDATA[1] = *((uint16_t *)data + 1);
       FlashRegs.FDATA[2] = *((uint16_t *)data + 2);
       FlashRegs.FDATA[3] = *((uint16_t *)data + 3);

       FlashRegs.FPROGCR.bit.FRES = 0x8;   // 启动编程
       while (FlashRegs.FSTATUS.bit.BUSY) { }
       EDIS;
   }

.. warning::

   Flash 操作注意事项：

   - 擦除/编程时必须工作在 Flash 中或 RAM 中 (不能在待擦除扇区执行代码)
   - Flash 擦除寿命约 1000 次 (擦除前需确认)
   - 编程时可以开启中断，但中断服务函数不能在目标区域
   - 擦除/编程后需重新配置等待状态
