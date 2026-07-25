================================
系统控制：时钟与初始化
================================

系统控制模块管理芯片的时钟、复位、看门狗和 CPU 定时器。

时钟系统
=========

.. code-block::

   ┌────────────┐    ┌──────────────┐    ┌─────────────┐
   │ 外部晶振    │    │  PLL 配置     │    │  CPU 时钟    │
   │ (OSC/XTAL) │───→│  PLLCR[DIV]  │───→│  CLKIN       │
   │ 20~35 MHz  │    │  + PLLSTS    │    │  150 MHz    │
   └────────────┘    └──────────────┘    └──────┬──────┘
                                                 │
                                        ┌────────┴────────┐
                                        │  外设时钟分频     │
                                        │  HSPCLK=75MHz   │
                                        │  LSPCLK=37.5MHz │
                                        └─────────────────┘

PLL 配置
---------

.. code-block:: c

   // F28335: 输入 30 MHz → 150 MHz CPU 时钟
   // 配置: PLLCR = 10 (×10 ÷ 2 = 5 倍频)
   // CLKIN = 30 MHz × (PLLCR / 2) = 150 MHz

   void InitPll(uint16_t pllcr, uint16_t pllsts)
   {
       // 1. 断开 PLL
       SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;  // 分频器 = /2

       // 2. 设置倍频
       SysCtrlRegs.PLLCR.bit.DIV = pllcr;  // ×10

       // 3. 等待 PLL 锁定
       while (SysCtrlRegs.PLLSTS.bit.PLLLOCKS != 1) { }

       // 4. 设置最终分频
       SysCtrlRegs.PLLSTS.bit.DIVSEL = 3;  // 不分频
   }

.. important::

   F28335 常用的时钟配置：

   .. list-table::
      :header-rows: 1

      * - 晶振 (MHz)
        - PLLCR
        - DIVSEL
        - CPU 时钟 (MHz)
      * - 20
        - 10
        - 3 (/1)
        - 100
      * - 30
        - 10
        - 3 (/1)
        - 150
      * - 20
        - 10
        - 2 (/2)
        - 50

外设时钟使能
--------------

.. code-block:: c

   void EnablePeripheralClocks(void)
   {
       EALLOW;

       // 使能 ePWM 时钟
       SysCtrlRegs.PCLKCR0.bit.EPWM1ENCLK = 1;
       SysCtrlRegs.PCLKCR0.bit.EPWM2ENCLK = 1;
       SysCtrlRegs.PCLKCR0.bit.ECAP3ENCLK = 1;

       // 使能 ADC 时钟（高速时钟 HSPCLK）
       SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;

       // 使能 SCI 时钟（低速时钟 LSPCLK）
       SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;
       SysCtrlRegs.PCLKCR0.bit.SCIBENCLK = 1;

       // 使能 SPI / I2C 时钟
       SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;
       SysCtrlRegs.PCLKCR1.bit.I2CAENCLK = 1;

       EDIS;
   }

.. warning::

   默认情况下所有外设时钟**都是关闭的**。
   使用外设前必须先使能对应的时钟。这是常见的遗漏点。

看门狗
=======

.. code-block:: c

   void DisableWatchdog(void)
   {
       EALLOW;
       SysCtrlRegs.WDCR = 0x00E8;  // WDPS=111 (/128), WDDIS=1(禁用)
       // Bit 7-6: 00 (保留)
       // Bit 5-3: 111 (分频 /128)
       // Bit 2-0: WDCHK = 101 必须写入 101 才有效
       EDIS;
   }

   void ServiceWatchdog(void)
   {
       EALLOW;
       SysCtrlRegs.WDKEY = 0x0055;  // 第一步
       SysCtrlRegs.WDKEY = 0x00AA;  // 第二步（必须在 512 个 OSCCLK 内）
       EDIS;
   }

.. code-block::

   看门狗定时周期：
   WDCLK = OSCCLK / (WDPS × 512)
   若 OSCCLK = 30 MHz, WDPS = /64:
   WDCLK = 30e6 / (64 × 512) ≈ 915 Hz
   超时 = 1 / 915 ≈ 1.09 ms

CPU 定时器
============

F28335 有三个 32 位 CPU 定时器（Timer 0/1/2），Timer 2 保留给实时操作系统。

.. code-block:: c

   struct CPUTIMER_VARS {
       volatile uint32_t  RegsAddr;
       uint32_t  CPUFreqInMHz;
       uint32_t  PeriodInUSec;
   };

   void InitCpuTimers(void)
   {
       // Timer 0: CPU 频率 150 MHz, 周期 1ms
       ConfigCpuTimer(&CpuTimer0, 150, 1000);
       CpuTimer0Regs.TCR.bit.TIE = 1;   // 使能中断
       CpuTimer0Regs.TCR.bit.TSS = 0;   // 启动
   }

   // 定时器中断服务函数
   interrupt void cpu_timer0_isr(void)
   {
       CpuTimer0.InterruptCount++;      // 每 1ms 递增
       PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;  // 应答中断
   }

.. note::

   - Timer 0：用户可用，可触发 PIE 中断
   - Timer 1：用户可用，不触发中断（轮询）
   - Timer 2：保留给 RTOS（如 TI-RTOS、SYS/BIOS）

系统初始化流程
===============

.. code-block:: c

   void main(void)
   {
       // 步骤 1：系统控制初始化
       InitSysCtrl();              // PLL + 看门狗 + 外设时钟

       // 步骤 2：GPIO 初始化
       InitGpio();                 // 配置 GPIO 功能

       // 步骤 3：PIE 中断初始化
       DINT;                       // 关闭全局中断
       InitPieCtrl();              // PIE 控制寄存器
       IER = 0x0000;              // 关闭 CPU 中断
       IFR = 0x0000;              // 清除标志
       InitPieVectTable();        // 初始化中断向量

       // 步骤 4：外设初始化
       InitAdc();
       InitEPwm();
       InitSci();

       // 步骤 5：用户初始化
       EALLOW;
       PieVectTable.TINT0 = &cpu_timer0_isr;  // 注册中断
       EDIS;
       PieCtrlRegs.PIEIER1.bit.INTx7 = 1;     // 使能 Timer 0
       IER |= M_INT1;                          // 使能 CPU INT1
       EINT;                                   // 开启全局中断

       // 步骤 6：主循环
       while (1) {
           // 后台任务
       }
   }
