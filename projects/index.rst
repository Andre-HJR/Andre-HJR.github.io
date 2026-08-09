=====================
开发项目
=====================

这里汇总 HJR 亲手完成的独立开发项目：从需求分析、逆向工程或协议定义起步，
到实现、测试与多语言封装，全程留档。每个项目的详细介绍见对应章节，下方卡片可直接跳转。

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: ⛏️ XTF-CORE
      :link: ../c/11-project-xtf-core
      :link-type: doc
      :text-align: center

      测井文件解析库
      ^^^^^^^^^^^^^^^^^
      纯 C89/C90 编写、零第三方依赖的 XTF 测井数据解析 / 生成库，
      支持 LAS 2.0 / 3.0 双向转换，附 Fortran / Python / C# 三种语言绑定。

      +++
      **语言：** 纯 C89/C90 · **规模：** 约 7,100 行 · **许可：** GPL-3.0

   .. grid-item-card:: 📱 蓝牙 MCU 控制器
      :link: ../flutter/09-project-bt-mcu-controller
      :link-type: doc
      :text-align: center

      Android BLE 串口助手
      ^^^^^^^^^^^^^^^^^^^^^^
      通过 BLE GATT 与单片机透传模块通信，文本 / HEX 双输入；
      协议帧与 CRC 由 NDK 原生 C 实现，不可用时自动回退纯 Dart。

      +++
      **框架：** Flutter 3.44 · **通信：** BLE 4.0+ · **许可：** GPL-3.0-or-later

项目一览
==========

.. list-table::
   :header-rows: 1

   * - 项目
     - 技术栈
     - 定位
     - 规模
     - 许可证
     - 章节
   * - :doc:`XTF-CORE <../c/11-project-xtf-core>`
     - 纯 C89/C90
     - ECLIPS 5700 XTF 测井文件解析 / 生成库
     - 约 7,100 行
     - GPL-3.0
     - :doc:`C 语言基础 <../c/index>`
   * - :doc:`蓝牙 MCU 控制器 <../flutter/09-project-bt-mcu-controller>`
     - Flutter + Dart + C（NDK）
     - Android BLE 串口助手
     - Dart 约 917 行 + C 118 行
     - GPL-3.0-or-later
     - :doc:`Flutter 基础 <../flutter/index>`

.. seealso::

   项目详细页分别位于 C 语言章节（:doc:`XTF-CORE <../c/11-project-xtf-core>`）
   与 Flutter 章节（:doc:`蓝牙 MCU 控制器 <../flutter/09-project-bt-mcu-controller>`）。
   每个项目页含完整的架构图、开发记录与踩坑复盘。
