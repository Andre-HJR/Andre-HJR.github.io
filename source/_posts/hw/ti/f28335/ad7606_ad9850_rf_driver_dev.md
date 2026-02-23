---
title: F28335的AD7606和AD9850的驱动开发
date: 2026-02-22 23:22:45
tags:
    - [HW, TI, F28335]
categories:
    - HW
    - TMS320F28335
---


## 关于 AD7606 和 AD9850 的 F28335 的驱动开发记录

<!-- more -->


## AD9850的驱动开发


### 目前的测试结果

<iframe src="//player.bilibili.com/player.html?isOutside=true&aid=116119574683604&bvid=BV1iYfiBkEpa&cid=36234921016&p=1" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"></iframe>

{% codeblock "初始化相关的寄存器 12-15" lang:cpp %}
void AD9850_GPIO_Init(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;

	GpioCtrlRegs.GPADIR.bit.GPIO12 = 1; // W_CLK
	GpioCtrlRegs.GPADIR.bit.GPIO13 = 1; // FQ_UD
	GpioCtrlRegs.GPADIR.bit.GPIO14 = 1; // DATA
	GpioCtrlRegs.GPADIR.bit.GPIO15 = 1; // RSET
	EDIS;
}
{% endcodeblock %}

{% codeblock "AD9850数据位写入" lang:cpp %}
void AD9850_WriteBit(Uint16 bit)
{
	GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;DELAY_US(10);
	if (bit)
		GpioDataRegs.GPASET.bit.GPIO14 = 1;
	else
		GpioDataRegs.GPACLEAR.bit.GPIO14 = 1;
	DELAY_US(10);
	GpioDataRegs.GPASET.bit.GPIO12 = 1;DELAY_US(10);
}
{% endcodeblock %}


{% codeblock "AD9850数据信息写入" lang:cpp %}
void AD9850_WriteData(Uint32 freq)
{
	Uint16 i;
	GpioDataRegs.GPACLEAR.bit.GPIO13 = 1;
    DELAY_US(10);
	for (i = 0; i < 32; ++i)
	{
		AD9850_WriteBit(freq & 0x01);
		freq >>= 1;
	}
	for(i = 0; i < 8; ++i)
	{
		AD9850_WriteBit(0);
	}
	GpioDataRegs.GPASET.bit.GPIO13 = 1;
    DELAY_US(10);
}
{% endcodeblock %}


{% codeblock "AD9850复位信号写入" lang:cpp %}
void AD9850_Reset(void)
{
	GpioDataRegs.GPASET.bit.GPIO15 = 1;
	DELAY_US(10);
	GpioDataRegs.GPACLEAR.bit.GPIO15 = 1;
}
{% endcodeblock %}

## 其他的驱动开发和调试


### LCD 的驱动开发


#### 问题

无法正确


