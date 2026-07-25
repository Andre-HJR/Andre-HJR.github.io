================================
ePWM 脉宽调制
================================

ePWM 是 F28335 最核心的外设之一，广泛应用于开关电源和电机控制。

ePWM 模块架构
===============

.. code-block::

   ePWM 模块内部结构：

   ┌──────────────┐    ┌──────────────┐
   │  时基 TB      │    │  计数比较 CC   │
   │  TBCTL       │    │  CMPA/CMPB   │
   │  TBPRD       │    │  CMPCTL      │
   │  TBCTR       │    │             │
   └──────┬───────┘    └──────┬───────┘
          │                   │
   ┌──────┴───────────────────┴───────┐
   │        动作限定器 AQ               │
   │  AQCTLA / AQCTLB / AQSFRC        │
   └──────┬────────────────────────────┘
          │
   ┌──────┴───────┐    ┌──────────────┐
   │  死区 DB      │    │  斩波器 PC    │
   │  DBCTL       │    │  PCCTL       │
   │  DBRED/DBFED  │    │             │
   └──────┬───────┘    └──────┬───────┘
          │                   │
   ┌──────┴───────────────────┴───────┐
   │        事件触发 ET                │
   │  ETSEL / ETPS / ETFLG           │
   └──────────────────────────────────┘

时基配置
=========

.. code-block:: c

   void EPwm1_Init(void)
   {
       // 1. 时基配置 (TB)
       EPwm1Regs.TBPRD = 1500;              // PWM 周期值
       // PWM 频率 = SYSCLKOUT / (TBPRD+1) × TBCLK 预分频
       // 100 kHz = 150 MHz / ((1500+1) × 1)
       // (TBCLK = SYSCLKOUT / HSPCLKDIV / CLKDIV)

       EPwm1Regs.TBPHS.bit.TBPHS = 0;       // 相位偏移
       EPwm1Regs.TBCTL.bit.CTRMODE = 0;     // 0=递增, 1=递减, 2=增减
       EPwm1Regs.TBCTL.bit.PHSEN = 0;       // 禁用相位加载
       EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;   // 高速时钟分频 (÷1)
       EPwm1Regs.TBCTL.bit.CLKDIV = 0;      // 时钟分频 (÷1)

       // 2. 计数比较 (CC)
       EPwm1Regs.CMPA.bit.CMPA = 750;       // 50% 占空比
       EPwm1Regs.CMPCTL.bit.SHDWAMODE = 0;  // 影子模式（自动加载）
       EPwm1Regs.CMPCTL.bit.LOADAMODE = 0;  // CTR=0 时加载

       // 3. 动作限定 (AQ)
       EPwm1Regs.AQCTLA.bit.CAU = 1;        // CTR=CMPA 增 → 输出低
       EPwm1Regs.AQCTLA.bit.CAD = 2;        // CTR=CMPA 减 → 输出高
       // (0=不动作, 1=低, 2=高, 3=翻转)
   }

.. code-block::

   输出波形示例（递增模式，CMPA=750，TBPRD=1500）：

   TBCTR
   │
   ├───                  ────
   │  │                 │   │
   │  │                 │   │
   │  │                 │   │
   │  │                 │   │
   ├──┴─────────────────┴───┴──→ t
   │  0         750        1500

   ePWM1A： ────              ────
                 │           │
                 └───────────┘
   (CAU=1, CAD=2 → 占空比 50%)

互补 PWM 与死区
=================

.. code-block:: c

   void EPwm1_Complementary_Init(void)
   {
       // 同上基础配置 + 死区设置

       // 4. 死区 (DB)
       EPwm1Regs.DBCTL.bit.OUT_MODE = 3;     // 两路都使能死区
       EPwm1Regs.DBCTL.bit.POLSEL = 2;       // 1: ePWMxA 不反转, ePWMxB 反转
       EPwm1Regs.DBRED.bit.DBRED = 50;       // 上升沿延迟 50 TBCLK
       EPwm1Regs.DBFED.bit.DBFED = 50;       // 下降沿延迟 50 TBCLK

       // 5. 动作限定 (两路独立)
       EPwm1Regs.AQCTLA.bit.CAU = 2;         // CTR=CMPA 增 → 高
       EPwm1Regs.AQCTLA.bit.CAD = 1;         // CTR=CMPA 减 → 低
       // ePWM1B 由死区模块从 ePWM1A 派生（无需单独配置 AQCTLB）
   }

.. code-block::

   互补输出（带死区）：

   ePWM1A ───────┐  ┌──────────────┐  ┌─────
                 │  │              │  │
                 │  │              │  │
                 └──┘              └──┘
   ePWM1B ──┐  ┌──┘              └──┐  ┌──
             │  │                    │  │
             │  │                    │  │
             └──┘                    └──┘

             ← 死区 →
             (DBRED/BFED)

HRPWM 高分辨率 PWM
===================

.. code-block:: c

   void HRPwm1_Init(void)
   {
       // HRPWM 可实现 ~150 ps 级的分辨率
       // 配置步骤：

       EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;   // MEP 需要 HSPCLK = SYSCLKOUT

       // 使能 HRPWM
       EPwm1Regs.HRCNFG.bit.HRMODE = 0;    // HR 模式（CMPA 高分辨率）
       EPwm1Regs.HRCNFG.bit.HRLOAD = 0;    // CTR=0 加载
       EPwm1Regs.HRCNFG.bit.AUTOCONV = 1;  // 自动转换
       EPwm1Regs.HRCNFG.bit.SWAP = 0;

       // 使用高分辨率 CMPA
       EPwm1Regs.CMPA.bit.CMPAHR = 0x80;   // 微调部分（8 位精度）
   }

.. note::

   HRPWM 利用 MEP（微边沿定位）技术，在传统 12~16 位 PWM 基础上
   额外提供约 8 位的高分辨率调整。非常适合数字电源的精细调节。

PWM 频率与分辨率对照
=======================

.. list-table::
   :header-rows: 1

   * - PWM 频率
     - 周期值 (150 MHz)
     - 常规分辨率 (位)
     - HRPWM 分辨率 (位)
   * - 20 kHz
     - 7500
     - 12.9
     - 20.9
   * - 50 kHz
     - 3000
     - 11.6
     - 19.6
   * - 100 kHz
     - 1500
     - 10.6
     - 18.6
   * - 200 kHz
     - 750
     - 9.6
     - 17.6
   * - 1 MHz
     - 150
     - 7.2
     - 15.2

PWM 中断
=========

.. code-block:: c

   void EPwm1_Int_Init(void)
   {
       // 事件触发 (ET) — 在指定事件时产生中断
       EPwm1Regs.ETSEL.bit.INTSEL = 1;     // CTR=0 时触发
       EPwm1Regs.ETSEL.bit.INTEN = 1;      // 使能中断
       EPwm1Regs.ETPS.bit.INTPRD = 1;      // 每个事件触发一次

       // 注册中断 (PIE 组 2, 通道 1)
       EALLOW;
       PieVectTable.EPWM1_INT = &epwm1_isr;
       EDIS;
       PieCtrlRegs.PIEIER2.bit.INTx1 = 1;
       IER |= M_INT2;
       EINT;
   }

   interrupt void epwm1_isr(void)
   {
       // 清除中断标志
       EPwm1Regs.ETCLR.bit.INT = 1;
       PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;

       // 电流环控制代码...
       // ADC 结果读取 → Clark/Park → PI → PWM 更新
   }
