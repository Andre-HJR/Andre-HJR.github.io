================================
调试与工具
================================

串口监视器
===========

.. code-block:: bash

   # IDF Monitor（支持颜色、时间戳、自动解码）
   idf.py monitor
   idf.py monitor -p COM3 -b 115200

   # 快捷键
   # Ctrl+]   退出
   # Ctrl+T   → H   帮助
   # Ctrl+T   → R  重启
   # Ctrl+T   → Z  暂停输出
   # Ctrl+T   → Y  启用/禁用时间戳
   # Ctrl+T   → F  Build & Flash & Monitor

   # 添加时间戳和颜色（默认已启用）
   idf.py monitor --timestamp

内存泄漏检测
=============

.. code-block:: c

   // 启用 Heap Tracing
   // menuconfig → Component config → Heap memory debugging

   #include "esp_heap_trace.h"

   #define NUM_RECORDS 100
   static heap_trace_record_t trace_records[NUM_RECORDS];

   void start_heap_tracing(void)
   {
       heap_trace_init_standalone(trace_records, NUM_RECORDS);
       heap_trace_start(HEAP_TRACE_LEAKS);
   }

   void dump_allocations(void)
   {
       heap_trace_dump();
       // 输出显示未释放的内存分配记录
   }

性能分析
=========

.. code-block:: c

   // 测量函数执行时间
   #include "esp_timer.h"

   void measure_performance(void)
   {
       int64_t start = esp_timer_get_time();

       // 要测量的代码
       do_something();

       int64_t elapsed = esp_timer_get_time() - start;
       ESP_LOGI(TAG, "耗时: %lld us", elapsed);
   }

   // 查看 CPU 利用率
   // menuconfig → FreeRTOS → Enable FreeRTOS trace stats

   void show_task_stats(void)
   {
       TaskStatus_t *task_array;
       UBaseType_t task_count = uxTaskGetNumberOfTasks();
       uint32_t total_run_time;

       task_array = malloc(sizeof(TaskStatus_t) * task_count);
       uxTaskGetSystemState(task_array, task_count, &total_run_time);

       for (int i = 0; i < task_count; i++) {
           printf("%s: CPU %d%%, 栈空闲 %d\n",
                  task_array[i].pcTaskName,
                  task_array[i].ulRunTimeCounter * 100 / total_run_time,
                  task_array[i].usStackHighWaterMark);
       }
       free(task_array);
   }

Flash 与 OTA 更新
==================

.. code-block:: bash

   # 查看 Flash 信息
   esptool.py --port COM3 flash_id

   # 备份固件
   esptool.py --port COM3 read_flash 0x0 0x400000 backup.bin

.. code-block:: c

   #include "esp_ota_ops.h"

   void ota_update(const char *url)
   {
       esp_http_client_config_t config = {
           .url = url,
           .timeout_ms = 30000,
       };
       esp_https_ota_config_t ota_config = {
           .http_config = &config,
       };

       esp_err_t ret = esp_https_ota(&ota_config);
       if (ret == ESP_OK) {
           esp_restart();  // OTA 成功后重启
       } else {
           ESP_LOGE(TAG, "OTA 失败: %s", esp_err_to_name(ret));
       }
   }

常用调试命令
=============

.. list-table::
   :header-rows: 1

   * - 命令
     - 功能
   * - ``idf.py build flash monitor``
     - 一键编译烧录监视
   * - ``idf.py size``
     - 查看固件大小
   * - ``idf.py size-components``
     - 查看各组件大小
   * - ``idf.py app``
     - 仅编译应用
   * - ``idf.py bootloader``
     - 仅编译 bootloader
   * - ``idf.py partition-table``
     - 生成分区表
   * - ``idf.py erase_flash``
     - 擦除整个 Flash
   * - ``idf.py reconfigure``
     - 重新配置（编译前需配置时使用）

常用芯片调试工具
=================

.. list-table::
   :header-rows: 1

   * - 工具
     - 用途
     - 安装方式
   * - esptool
     - 烧录/擦除/读取 Flash
     - ``pip install esptool``
   * - idf-monitor
     - 串口监视器（IDF 内置）
     - 随 IDF 安装
   * - OpenOCD
     - JTAG 调试
     - ``idf.py openocd``
   * - Wireshark
     - WiFi 抓包分析
     - 官网下载
   * - esp_log
     - 日志系统
     - 内置

常见问题
=========

.. list-table::
   :header-rows: 1

   * - 现象
     - 原因
     - 解决
   * - 烧录后没有输出
     - 串口波特率不对
     - 确认 monitor 波特率 = 固件设置 (通常 115200)
   * - 反复重启
     - 看门狗复位 / 电源不稳
     - 检查电源 3.3V 输出 / 增加看门狗超时
   * - WiFi 连接失败
     - 信道不匹配 / 密码错误
     - 检查 WiFi 配置，增加日志级别
   * - 内存不足
     - 堆碎片 / 内存泄漏
     - 使用 heap tracing 排查
   * - BLE 广播不可见
     - 天线匹配 / 功率设置
     - 检查天线，增加 TX 功率
