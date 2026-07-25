================================
ADC 与 eCAP/eQEP
================================

ADC 模块
=========

F28335 的 ADC 模块：12 位精度、16 通道、80 ns 转换时间。

.. code-block::

   ADC 模块结构：

                         ┌────────────┐
   ADCINA0 ──────────→   │            │
   ADCINA1 ──────────→   │  8 选 1    │
   ...                   │  MUX       │──→ S/H ─→ 12-bit ADC ─→ RESULT0
   ADCINA7 ──────────→   │            │
                         └────────────┘
                         ┌────────────┐
   ADCINB0 ──────────→   │            │
   ...                   │  8 选 1    │──→ S/H ─→ 12-bit ADC ─→ RESULT1
   ADCINB7 ──────────→   │            │
                         └────────────┘

ADC 初始化
-----------

.. code-block:: c

   void InitAdc(void)
   {
       // 1. 使能 ADC 时钟
       SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;

       // 2. 上电校准
       AdcRegs.ADCTRL3.all = 0x00E0;  // ADC 上电，带隙参考使能
       DELAY_US(1000);                // 等待稳定

       // 3. 设置采样窗口
       AdcRegs.ADCTRL1.bit.ACQ_PS = 15;   // 采样窗口 = 16 个 ADCCLK
       // ADC 时钟 = HSPCLK / (ADCTRL3.ADCCLKPS + 1)
       // 典型值：150 MHz / (6+1) ≈ 21.4 MHz → 46.7 ns

       // 4. 设置转换模式
       AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;   // 级联模式（16 通道排序器）
       AdcRegs.ADCTRL1.bit.CONT_RUN = 0;   // 启动-停止模式
       AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1; // ePWM SOCA 触发
       AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;   // 使能 SEQ1 中断
   }

排序器配置
-----------

.. code-block:: c

   // 自动顺序转换 4 个通道
   void Adc_ConfigChannels(void)
   {
       // 设置采样通道顺序
       AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0;   // ADCINA0
       AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 1;   // ADCINA1
       AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 2;   // ADCINA2
       AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 3;   // ADCINA3

       // 设置转换次数
       AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 3;  // 4 次转换 (0~3)
   }

读取结果
---------

.. code-block:: c

   interrupt void adc_isr(void)
   {
       uint16_t sample[4];

       // 读取结果（结果寄存器自动递增）
       sample[0] = AdcRegs.ADCRESULT0 >> 4;   // 右对齐→右移4位
       sample[1] = AdcRegs.ADCRESULT1 >> 4;
       sample[2] = AdcRegs.ADCRESULT2 >> 4;
       sample[3] = AdcRegs.ADCRESULT3 >> 4;

       // 转换为电压（0~4095 → 0~3.3V）
       float voltage = (sample[0] * 3.3) / 4095.0;

       // 清除标志
       AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
       PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
   }

eCAP 捕获模块
===============

eCAP 用于测量 PWM 信号的频率、占空比和相位差。

.. code-block::

   eCAP 工作模式（4 种）：

   模式 1：捕获模式
      捕获输入信号的边沿，记录时间戳
      用于测量频率/占空比

   模式 2：APWM 模式
      作为简易 PWM 发生器使用

eCAP 捕获测量频率
------------------

.. code-block:: c

   void ECap1_Init(void)
   {
       // 捕获模式配置
       ECap1Regs.ECEINT.all = 0x0000;     // 先关闭中断
       ECap1Regs.ECCLR.all = 0xFFFF;      // 清除标志

       ECap1Regs.ECCTL1.bit.CAPLDEN = 1;  // 使能捕获加载
       ECap1Regs.ECCTL1.bit.PRESCALE = 0; // 预分频系数 ÷1
       ECap1Regs.ECCTL1.bit.CAP1POL = 0;  // 上升沿触发
       ECap1Regs.ECCTL1.bit.CAP2POL = 1;  // 下降沿触发
       ECap1Regs.ECCTL1.bit.CAP3POL = 1;  // 下降沿
       ECap1Regs.ECCTL1.bit.CAP4POL = 0;  // 上升沿

       ECap1Regs.ECCTL2.bit.CONT_ONESHT = 1;  // 单次触发
       ECap1Regs.ECCTL2.bit.STOP_WRAP = 3;    // 4 个事件后停止
       ECap1Regs.ECCTL2.bit.REARM = 1;        // 强制重新触发
       ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;    // 启动计数器

       // 中断配置
       ECap1Regs.ECEINT.bit.CEVT4 = 1;    // 第 4 个捕获事件触发中断
   }

   // 计算频率
   float ECap1_GetFrequency(void)
   {
       uint32_t period = ECap1Regs.CAP1;   // 周期计数值
       float freq = 150e6 / (float)period; // 频率 = SYSCLKOUT / period
       return freq;
   }

eQEP 正交编码器接口
=====================

eQEP 连接电机编码器，获取位置和速度信息。

.. code-block::

   QEP 信号：

   QEPA ──┤ ├────┤ ├────┤ ├────   (A 相)
   QEPB ──┤ ├──┤ ├────┤ ├──┤ ├─  (B 相，相位差 90°)
   QEPI ──┤ ├───────────────────  (Z 相索引/零位)

.. code-block:: c

   void EQep1_Init(void)
   {
       // 正交计数模式
       EQep1Regs.QUPRD = 1500;            // 速度计算周期（单位：CPU 时钟）
       EQep1Regs.QDECCTL.bit.QSRC = 0;    // 正交模式（QA+QB 输入）
       EQep1Regs.QDECCTL.bit.XCR = 0;     // 2x 分辨率（提升 2 倍）
       EQep1Regs.QDECCTL.bit.SWAP = 0;    // 不交换 QA/QB

       // 位置计数器
       EQep1Regs.QPOSINIT = 0;            // 初始位置
       EQep1Regs.QPOSMAX = 0xFFFFFFFF;    // 最大位置

       // 使能中断（位置到达或速度计算完成）
       EQep1Regs.QEINT.bit.QDC = 1;       // 方向变化中断
       EQep1Regs.QEINT.bit.QFLG = 1;
       EQep1Regs.QCAPCTL.bit.UPPS = 5;    // 单位位置事件预分频
       EQep1Regs.QCAPCTL.bit.CCPPS = 7;   // 捕获时钟预分频
       EQep1Regs.QCAPCTL.bit.CEN = 1;     // 使能位置捕获
   }

   // 读取位置
   int32_t EQep1_GetPosition(void)
   {
       return EQep1Regs.QPOSCNT;          // 当前计数值
   }

   // 读取速度 (rpm)
   float EQep1_GetSpeed(void)
   {
       uint32_t pulse = EQep1Regs.QPOSLAT; // 最后捕获的位置差
       // speed_rpm = pulse × 60 / (encoder_ppr × QUPRD × 150e6)
       return pulse * 60.0 / (4000.0 * 1500 * 150e-6);
   }
