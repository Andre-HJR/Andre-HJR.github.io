================================
GPIO 与 PIE 中断系统
================================

F28335 有 88 个 GPIO 引脚，每个引脚可配置为通用 I/O 或外设功能。

GPIO 配置
==========

.. code-block:: c

   // GPIO 模式设置
   // 每个 GPIO 有 4 种功能（通过 GPAMUX1/2 或 GPBMUX1/2 配置）

   void Gpio_setup(void)
   {
       EALLOW;

       // GPIO0：通用输出
       GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;   // 00 = GPIO 模式
       GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;    // 输出
       GpioCtrlRegs.GPAQSEL1.bit.GPIO0 = 0;  // 同步采样（输入滤波）

       // GPIO1：通用输入（上拉）
       GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;
       GpioCtrlRegs.GPADIR.bit.GPIO1 = 0;    // 输入
       GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;    // 使能上拉（0=启用）

       // GPIO2：ePWM1A 外设功能
       GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;   // 01 = ePWM1A

       // GPIO3：SCI 接收
       GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;   // 01 = SCIRXDA

       EDIS;
   }

.. list-table::
   :header-rows: 1

   * - GPAMUX 值
     - 功能
   * - 00
     - 通用 GPIO
   * - 01
     - 外设功能 1
   * - 10
     - 外设功能 2
   * - 11
     - 外设功能 3

GPIO 读写
==========

.. code-block:: c

   // 写输出
   GpioDataRegs.GPADAT.bit.GPIO0 = 1;    // 高电平
   GpioDataRegs.GPADAT.bit.GPIO0 = 0;    // 低电平

   // 推荐：仅翻转指定位（不影响其他引脚）
   GpioDataRegs.GPASET.bit.GPIO0 = 1;    // 置位
   GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;  // 清零
   GpioDataRegs.GPATOGGLE.bit.GPIO0 = 1; // 翻转

   // 读输入（不管 GPIO 方向，都能读取实际引脚电平）
   uint16_t pin = GpioDataRegs.GPADAT.bit.GPIO1;

PIE 中断系统
==============

F28335 的 PIE（外设中断扩展）模块将 96 个外设中断复用到 12 个 CPU 中断。

.. code-block::

   PIE 中断结构：

   ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
   │  外设中断源   │────→│   PIE 分组    │────→│   CPU 中断    │
   │  (96个)       │     │  (12组×8路)  │     │  INT1~INT12  │
   └──────────────┘     └──────────────┘     └──────────────┘

   PIE 分组示意：

   INT1:  TIMER0  | ADCINT  | ...
   INT2:  EPWM1   | EPWM2   | ...
   INT3:  EPWM3   | EPWM4   | ...
   INT4:  ECAP1   | ECAP2   | ...
   INT5:  EQEP1   | ...
   INT6:  SPIRX   | SPITX   | ...
   INT7:  ...
   INT8:  I2C     | ...
   INT9:  SCIRX   | SCITX   | ...
   INT10: CAN     | ...
   INT11: CLA     | ...
   INT12: XINT1-3 | ...

中断配置步骤
--------------

.. code-block:: c

   // 1. 中断服务函数声明
   interrupt void adc_isr(void);

   // 2. 初始化 PIE 并注册中断服务
   void InitInterrupts(void)
   {
       // 初始化 PIE 控制寄存器
       InitPieCtrl();

       // 关闭所有 CPU 中断
       IER = 0x0000;
       IFR = 0x0000;

       // 初始化中断向量表
       InitPieVectTable();

       // 注册中断服务函数
       EALLOW;
       PieVectTable.ADCINT1 = &adc_isr;
       EDIS;

       // 使能 PIE 分组中的具体中断
       PieCtrlRegs.PIEIER1.bit.INTx1 = 1;  // PIE 组 1 的第 1 个中断 (ADC)

       // 使能 CPU 中断
       IER |= M_INT1;   // CPU 中断 1

       // 开启全局中断
       EINT;            // 相当于 asm(" CLRC INTM")
   }

   // 3. 实现中断服务函数
   interrupt void adc_isr(void)
   {
       static uint16_t sample;

       // 读取 ADC 结果
       sample = AdcRegs.ADCRESULT0;

       // 清除 ADC 中断标志
       AdcRegs.ADCST.bit.INT_FLAG_CLR = 1;

       // 应答 PIE 中断
       PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

       // CPU 自动清除 INTM（中断返回时会自动恢复）
   }

.. important::

   中断服务函数必须：

   1. 清除外设中断标志 (每个外设不同)
   2. **应答 PIE 中断**：``PieCtrlRegs.PIEACK.all = PIEACK_GROUPn``
   3. 使用 ``interrupt`` 关键字声明
   4. 代码尽量简短（避免长时间关中断）
   5. 函数名注册到 ``PieVectTable``

外部中断（XINT1~XINT3）
=========================

.. code-block:: c

   void InitExternalInterrupt(void)
   {
       // XINT1 连接到 GPIO0，下降沿触发
       EALLOW;
       GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 0;  // GPIO0
       EDIS;

       XIntruptRegs.XINT1CR.bit.POLARITY = 1;      // 下降沿
       XIntruptRegs.XINT1CR.bit.ENABLE = 1;        // 使能

       // 注册中断
       EALLOW;
       PieVectTable.XINT1 = &xint1_isr;
       EDIS;
       PieCtrlRegs.PIEIER12.bit.INTx1 = 1;
       IER |= M_INT12;
       EINT;
   }

   interrupt void xint1_isr(void)
   {
       XIntruptRegs.XINT1CR.bit.ENABLE = 1;  // 清除标志
       PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
   }

中断优先级
===========

.. list-table::
   :header-rows: 1

   * - 优先级
     - CPU 中断
     - 典型中断源
   * - 最高
     - INT1
     - PDPINTA, ADCINT, TINT0
   * - ↓
     - INT2~INT4
     - ePWM, eCAP
   * - ↓
     - INT5~INT8
     - eQEP, SPI, I2C
   * - ↓
     - INT9~INT11
     - SCI, CAN, CLA
   * - 最低
     - INT12
     - XINT1~3, 保留

.. tip::

   电机控制中常见的中断优先级分配：

   1. **最高**：故障捕获 (PDPINT) — 立即关 PWM
   2. **高**：ePWM 中断 — 电流环 (10~20 kHz)
   3. **中**：ADC 中断 — 采样完成
   4. **低**：SCI/CAN 中断 — 通信
   5. **最低**：CPU Timer — 后台任务
