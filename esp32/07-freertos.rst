================================
FreeRTOS 任务管理
================================

ESP-IDF 基于 FreeRTOS，多任务编程是 ESP32 的核心能力。

任务创建
=========

.. code-block:: c

   #include "freertos/FreeRTOS.h"
   #include "freertos/task.h"

   // 任务函数原型：void task_name(void *pvParameter)

   void blink_task(void *pvParameter)
   {
       gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

       while (1) {
           gpio_set_level(GPIO_NUM_2, 1);
           vTaskDelay(500 / portTICK_PERIOD_MS);
           gpio_set_level(GPIO_NUM_2, 0);
           vTaskDelay(500 / portTICK_PERIOD_MS);
       }
   }

   void app_main(void)
   {
       // 创建任务
       xTaskCreate(
           blink_task,          // 任务函数
           "blink",             // 任务名
           2048,                // 栈大小（字）
           NULL,                // 参数
           5,                   // 优先级 (0~25)
           NULL                 // 任务句柄
       );
   }

.. note::

   ESP-IDF 中 FreeRTOS 的优先级范围是 **0~25**，数字越大优先级越高。
   空闲任务优先级为 0，Timer 服务任务优先级为 1。

多核任务
=========

ESP32 的双核可以分配任务到特定核心：

.. code-block:: c

   // 创建任务并绑定到核心 0 (PRO_CPU)
   xTaskCreatePinnedToCore(
       task_function,
       "task_pro",
       4096,
       NULL,
       5,
       NULL,
       0);                     // 核心 0

   // 绑定到核心 1 (APP_CPU)
   xTaskCreatePinnedToCore(
       task_function,
       "task_app",
       4096,
       NULL,
       5,
       NULL,
       1);                     // 核心 1

.. important::

   - 核心 0 (PRO_CPU)：处理 WiFi/BLE 协议栈、中断
   - 核心 1 (APP_CPU)：运行用户应用代码
   - 如果没有指定核心，IDF 默认使用核心 1

队列通信
=========

.. code-block:: c

   // 创建队列
   QueueHandle_t sensor_queue;
   sensor_queue = xQueueCreate(10, sizeof(float));

   // 发送方（可在 ISR 中使用）
   float temperature = 25.5;
   xQueueSend(sensor_queue, &temperature, portMAX_DELAY);

   // 接收方
   float received_temp;
   if (xQueueReceive(sensor_queue, &received_temp,
                     pdMS_TO_TICKS(1000)) == pdTRUE) {
       ESP_LOGI(TAG, "温度: %.1f", received_temp);
   }

信号量
=======

.. code-block:: c

   SemaphoreHandle_t xSemaphore;

   // 二值信号量（同步）
   xSemaphore = xSemaphoreCreateBinary();
   xSemaphoreGive(xSemaphore);                    // 释放
   xSemaphoreTake(xSemaphore, portMAX_DELAY);     // 获取（阻塞）

   // 互斥量（带优先级继承）
   SemaphoreHandle_t xMutex = xSemaphoreCreateMutex();

   void shared_resource_task(void *pvParameter)
   {
       while (1) {
           if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
               // 访问共享资源
               critical_section_operation();
               xSemaphoreGive(xMutex);
           }
           vTaskDelay(10 / portTICK_PERIOD_MS);
       }
   }

事件组
=======

.. code-block:: c

   EventGroupHandle_t event_group;
   const int BIT_SENSOR_READY = BIT0;
   const int BIT_WIFI_READY   = BIT1;

   void event_example(void)
   {
       event_group = xEventGroupCreate();

       // 设置事件位
       xEventGroupSetBits(event_group, BIT_SENSOR_READY);
       xEventGroupSetBits(event_group, BIT_WIFI_READY);

       // 等待多个事件（所有位）
       EventBits_t bits = xEventGroupWaitBits(
           event_group,
           BIT_SENSOR_READY | BIT_WIFI_READY,
           pdTRUE,      // 清除位
           pdTRUE,      // 等待所有位
           portMAX_DELAY);
   }

任务通知
=========

.. code-block:: c

   // 轻量级同步（比信号量更快，更省内存）

   TaskHandle_t task_handle;

   // 发送通知
   xTaskNotifyGive(task_handle);

   // 接收通知（在目标任务中）
   ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

   // 带值的通知
   uint32_t notify_value = 42;
   xTaskNotify(task_handle, notify_value, eSetValueWithOverwrite);

   // 接收
   uint32_t value;
   xTaskNotifyWait(0, 0, &value, portMAX_DELAY);

软件定时器
===========

.. code-block:: c

   TimerHandle_t timer;

   void timer_callback(TimerHandle_t xTimer)
   {
       ESP_LOGI(TAG, "定时器触发");
   }

   void timer_create(void)
   {
       timer = xTimerCreate(
           "my_timer",              // 名字
           pdMS_TO_TICKS(1000),     // 周期
           pdTRUE,                  // 自动重载
           NULL,                    // ID
           timer_callback);

       xTimerStart(timer, 0);       // 启动
       // xTimerStop(timer, 0);     // 停止
   }

栈溢出检测
===========

.. code-block:: c

   // 方法 1：配置 menuconfig
   // Component config → FreeRTOS → configCHECK_FOR_STACK_OVERFLOW

   // 方法 2：运行时检查
   UBaseType_t stack_high_water = uxTaskGetStackHighWaterMark(NULL);
   ESP_LOGI(TAG, "剩余栈空间: %d 字", stack_high_water * 4);

   // 如果剩余栈空间 < 256 字节，应增加任务栈大小

.. tip::

   FreeRTOS 使用建议：

   1. 栈大小: 任务创建时分配，默认 2048 字 (~8 KB)，根据需求调整
   2. **优先级**：避免同优先级任务长时间占用 CPU（用 vTaskDelay 让出）
   3. **ISR**：中断服务函数应尽量短，复杂处理通过队列/信号量交给任务
   4. **看门狗**：长时间循环记得喂狗 `esp_task_wdt_reset()`
   5. **内存**：使用 `heap_caps_get_free_size(MALLOC_CAP_INTERNAL)` 监控内存
