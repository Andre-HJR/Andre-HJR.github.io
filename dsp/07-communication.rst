================================
通信接口
================================

F28335 提供 SCI (UART)、SPI、I2C 和 eCAN 等通信接口。

SCI (UART)
===========

SCI 是异步串行接口，支持 RS232/RS485。

.. code-block:: c

   void SciA_Init(void)
   {
       // 配置 GPIO（使用功能 1）
       EALLOW;
       GpioCtrlRegs.GPAMUX1.bit.GPIO28 = 1;  // SCIRXDA
       GpioCtrlRegs.GPAMUX1.bit.GPIO29 = 1;  // SCITXDA
       EDIS;

       // SCI 配置：115200, 8N1
       SciaRegs.SCICCR.all = 0x0007;   // 8 位, 无校验, 1 停止位
       SciaRegs.SCICTL1.all = 0x0003;  // 使能 TX, RX
       SciaRegs.SCICTL2.bit.TXINTENA = 1;
       SciaRegs.SCICTL2.bit.RXBKINTENA = 1;

       // 波特率 = LSPCLK / (BRR + 1) × 8
       // 115200 = 37.5e6 / (BRR + 1) × 8
       // BRR ≈ 39
       SciaRegs.SCIHBAUD = 0x0000;
       SciaRegs.SCILBAUD = 39;         // 115200 baud

       SciaRegs.SCICTL1.all = 0x0023;  // 退出复位
   }

.. code-block:: c

   // 发送字符
   void SciA_SendChar(char data)
   {
       while (SciaRegs.SCICTL2.bit.TXRDY == 0) { }  // 等待 TX 缓冲区空
       SciaRegs.SCITXBUF = data;
   }

   // 发送字符串
   void SciA_SendString(const char *str)
   {
       while (*str) {
           SciA_SendChar(*str++);
       }
   }

   // 接收字符（中断方式）
   interrupt void scia_rx_isr(void)
   {
       uint16_t received = SciaRegs.SCIRXBUF.all;  // 读取数据

       // 处理接收到的数据
       rx_buffer[rx_index++] = received & 0xFF;

       // 清除标志
       PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
   }

SPI
====

.. code-block:: c

   void SpiA_Init(void)
   {
       // GPIO 配置
       EALLOW;
       GpioCtrlRegs.GPAMUX1.bit.GPIO16 = 1;  // SPISIMO
       GpioCtrlRegs.GPAMUX1.bit.GPIO17 = 1;  // SPISOMI
       GpioCtrlRegs.GPAMUX1.bit.GPIO18 = 1;  // SPICLK
       GpioCtrlRegs.GPAMUX1.bit.GPIO19 = 1;  // SPISTE（片选）
       EDIS;

       // SPI 配置
       SpiaRegs.SPICCR.all = 0x000F;    // 16 位数据, 上升沿
       SpiaRegs.SPICTL.all = 0x0006;    // 主模式, 使能 TX
       SpiaRegs.SPIBRR = 0x004E;        // 波特率 = LSPCLK / (BRR+1)
       SpiaRegs.SPICCR.all = 0x009F;    // 退出复位, 16 位

       // 片选控制 (GPIO19 作为 SPI STE)
       SpiaRegs.SPICTL.bit.SPIINTENA = 0;  // 暂不使用中断
   }

   uint16_t SpiA_Transfer(uint16_t data)
   {
       SpiaRegs.SPITXBUF = data;            // 发送
       while (SpiaRegs.SPISTS.bit.INT_FLAG == 0) { }  // 等待完成
       return SpiaRegs.SPIRXBUF;            // 返回接收数据
   }

   // SPI 写入 DAC 示例
   void SpiA_WriteDAC(uint16_t channel, uint16_t value)
   {
       uint16_t cmd = (channel << 15) | (value & 0x0FFF);
       SpiaRegs.SPITXBUF = cmd;
       while (SpiaRegs.SPISTS.bit.INT_FLAG == 0) { }
       SpiaRegs.SPIRXBUF;  // 读取清除标志
   }

I2C
====

.. code-block:: c

   void I2cA_Init(void)
   {
       // GPIO 配置
       EALLOW;
       GpioCtrlRegs.GPAMUX1.bit.GPIO32 = 1;  // SDAA
       GpioCtrlRegs.GPAMUX1.bit.GPIO33 = 1;  // SCLA
       EDIS;

       // I2C 配置
       I2caRegs.I2CSAR = 0x0050;       // 从机地址 (7-bit)
       I2caRegs.I2CCLKL = 60;          // SCL 低电平计数
       I2caRegs.I2CCLKH = 60;          // SCL 高电平计数
       // I2C 频率 = LSPCLK / (I2CCLKL + I2CCLKH + 6)
       // 100 kHz = 37.5e6 / (60 + 60 + 6)
       I2caRegs.I2CPSC = 18;           // 预分频 = LSPCLK / (PSC+1)
       I2caRegs.I2CMDR.bit.IRS = 1;    // 退出复位
   }

   void I2cA_WriteByte(uint16_t reg, uint16_t data)
   {
       // 等待总线空闲
       while (I2caRegs.I2CSTR.bit.BB == 1) { }

       I2caRegs.I2CSAR = 0x50;          // 从机地址
       I2caRegs.I2CCNT = 2;             // 发送 2 字节（寄存器+数据）
       I2caRegs.I2CDXR = reg;           // 寄存器地址
       I2caRegs.I2CDXR = data;          // 数据

       I2caRegs.I2CMDR.all = 0x0620;    // 主发模式, 起始位, 停止位
       while (I2caRegs.I2CSTR.bit.SCD == 0) { }  // 等待停止位
       I2caRegs.I2CSTR.bit.SCD = 1;     // 清除标志
   }

eCAN
====

.. code-block:: c

   void CAN_Init(void)
   {
       EALLOW;
       GpioCtrlRegs.GPAMUX2.bit.GPIO34 = 1;  // CANTXB
       GpioCtrlRegs.GPAMUX2.bit.GPIO35 = 1;  // CANRXB
       EDIS;

       // 初始化 eCAN
       ECanaRegs.CANMC.bit.SCB = 1;      // 使能 eCAN 模式
       ECanaRegs.CANMC.bit.ABO = 1;      // 自动恢复（总线关闭后）
       ECanaRegs.CANBTC.bit.BRPREG = 4;  // 波特率预分频
       ECanaRegs.CANBTC.bit.TSEG2REG = 2;
       ECanaRegs.CANBTC.bit.TSEG1REG = 5;
       // 波特率 = SYSCLKOUT / ((BRPREG+1) × (TSEG1+ TSEG2+1))
       // 500 kbps = 150 MHz / (5 × (5+2+1) × 2)

       ECanaRegs.CANMC.bit.CCR = 1;      // 进入配置模式
       while (ECanaRegs.CANES.bit.CCE != 1) { }
       ECanaRegs.CANMC.bit.CCR = 0;      // 退出配置模式
       while (ECanaRegs.CANES.bit.CCE == 1) { }
   }

   void CAN_SendMessage(uint32_t id, uint16_t *data, uint8_t len)
   {
       // 等待邮箱就绪
       while (ECanaRegs.CANTRS.all != 0) { }

       ECanaMboxes.MBOX0.MSGID.all = id & 0x1FFFFFFF;
       ECanaMboxes.MBOX0.MSGCTRL.all = 0;

       // 复制数据
       for (int i = 0; i < len; i++) {
           ECanaMboxes.MBOX0.MDR[i].all = data[i];
       }

       ECanaRegs.CANTRS.bit.TRS0 = 1;   // 请求发送
   }

.. tip::

   CAN 总线常用于电机驱动和工业控制：
   - **500 kbps**：最长 250 米
   - **250 kbps**：最长 500 米
   - **125 kbps**：最长 1000 米
