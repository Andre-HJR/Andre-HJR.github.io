================================
ESP-IDF 环境搭建
================================

ESP-IDF（Espressif IoT Development Framework）是乐鑫官方 SDK，基于 FreeRTOS。

安装 ESP-IDF
=============

方式一：VSCode 插件（推荐）
---------------------------

.. code-block::

   1. 安装 VSCode
   2. 安装扩展：Espressif IDF (by Espressif Systems)
   3. 按 F1 → "ESP-IDF: Configure ESP-IDF Extension"
   4. 选择 "Advanced" → 选择版本 (v5.3 或 latest)
   5. 等待自动下载安装（约 15~30 分钟）

方式二：命令行安装（Windows）
------------------------------

.. code-block:: bash

   # 安装依赖 (Python、Git 需提前安装)
   git clone -b v5.3 --recursive https://github.com/espressif/esp-idf.git

   # 运行安装脚本
   cd esp-idf
   .\install.bat

   # 设置环境变量（每次新终端需执行）
   .\export.bat

方式三：Linux / macOS
-----------------------

.. code-block:: bash

   git clone -b v5.3 --recursive https://github.com/espressif/esp-idf.git
   cd esp-idf
   ./install.sh
   source ./export.sh   # 添加到 ~/.bashrc 可永久生效

创建第一个项目
===============

.. code-block:: bash

   # 复制模板项目
   cp -r $IDF_PATH/examples/get-started/hello_world .

   # 配置项目
   cd hello_world
   idf.py set-target esp32                # 选择芯片
   idf.py menuconfig                       # 菜单配置

   # 编译
   idf.py build

   # 烧录 (连接 USB)
   idf.py -p COM3 flash

   # 监视串口输出
   idf.py -p COM3 monitor

.. code-block:: c
   :caption: hello_world/main/hello_world_main.c

   #include <stdio.h>
   #include "freertos/FreeRTOS.h"
   #include "freertos/task.h"
   #include "esp_system.h"
   #include "esp_log.h"

   static const char *TAG = "MAIN";

   void app_main(void)
   {
       ESP_LOGI(TAG, "ESP32 启动成功!");
       ESP_LOGI(TAG, "芯片版本: %d", esp_chip_info(&chip_info));

       while (1) {
           ESP_LOGI(TAG, "正常运行中...");
           vTaskDelay(1000 / portTICK_PERIOD_MS);
       }
   }

项目结构
=========

.. code-block::

   my_project/
   ├── CMakeLists.txt           # 顶层 CMake 配置
   ├── sdkconfig                # 编译配置 (menuconfig 生成)
   ├── main/
   │   ├── CMakeLists.txt       # 源文件列表
   │   ├── app_main.c           # 入口文件
   │   └── component.mk         # (旧版 Make 兼容)
   ├── components/              # 自定义组件
   │   └── my_component/
   │       ├── CMakeLists.txt
   │       └── my_component.c
   ├── build/                   # 编译产物
   ├── partitions.csv           # 分区表
   └── flash_args               # 烧录参数

.. code-block:: cmake
   :caption: main/CMakeLists.txt

   idf_component_register(
       SRCS "app_main.c" "gpio_control.c"
       INCLUDE_DIRS "."
       REQUIRES driver nvs_flash
   )

常用 idf.py 命令
==================

.. list-table::
   :header-rows: 1

   * - 命令
     - 说明
   * - ``idf.py set-target esp32``
     - 设置目标芯片
   * - ``idf.py menuconfig``
     - 图形化配置
   * - ``idf.py build``
     - 编译
   * - ``idf.py flash``
     - 烧录
   * - ``idf.py monitor``
     - 串口监视器
   * - ``idf.py clean``
     - 清理编译产物
   * - ``idf.py fullclean``
     - 完全清理
   * - ``idf.py build flash monitor``
     - 一条龙（编译+烧录+监视）
   * - ``idf.py size``
     - 查看固件大小
   * - ``idf.py size-components``
     - 按组件查看大小

分区表
=======

.. code-block::
   :caption: partitions.csv

   # Name,   Type, SubType, Offset,  Size, Flags
   nvs,      data, nvs,     0x9000,  0x4000,
   otadata,  data, ota,     0xd000,  0x2000,
   app0,     app,  ota_0,   0x10000, 0x1E0000,
   app1,     app,  ota_1,   0x1F0000,0x1E0000,
   spiffs,   data, spiffs,  0x3D0000,0x20000,
   coredump, data, coredump,0x3F0000,0x10000,

.. note::

   分区表决定了 Flash 布局。OTA 分区需要至少两个 app 分区（app0 + app1）。
   自定义分区可通过 menuconfig → Partition Table 选择。

常见问题
=========

.. list-table::
   :header-rows: 1

   * - 问题
     - 解决
   * - 烧录时报 ``A fatal error occurred: Failed to connect to ESP32``
     - 按住 BOOT 键再按一下 EN（或 RST）再松开 BOOT
   * - ``idf.py: command not found``
     - 运行 ``export.bat`` 或 ``source export.sh``
   * - 串口打不开 (Windows)
     - 安装 CP2102 / CH340 驱动
   * - 编译速度慢
     - ``idf.py build -j4``，即指定并行任务数
