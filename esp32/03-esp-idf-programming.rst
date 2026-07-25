===============================
ESP-IDF 编程基础
===============================

日志系统
=========

.. code-block:: c

   #include "esp_log.h"

   static const char *TAG = "APP";

   ESP_LOGE(TAG, "错误信息");   // Error (红色)
   ESP_LOGW(TAG, "警告信息");   // Warning
   ESP_LOGI(TAG, "普通信息");   // Info
   ESP_LOGD(TAG, "调试信息");   // Debug (需开启)
   ESP_LOGV(TAG, "冗余信息");   // Verbose

   // 设置日志级别
   // menuconfig → Component config → Log output → Default log verbosity

   // 运行时动态调整
   esp_log_level_set(TAG, ESP_LOG_VERBOSE);

GPIO 操作
=========

.. code-block:: c

   #include "driver/gpio.h"

   #define GPIO_LED    GPIO_NUM_2
   #define GPIO_BUTTON GPIO_NUM_0

   void gpio_init(void)
   {
       // 配置输出
       gpio_reset_pin(GPIO_LED);
       gpio_set_direction(GPIO_LED, GPIO_MODE_OUTPUT);

       // 配置输入（上拉）
       gpio_reset_pin(GPIO_BUTTON);
       gpio_set_direction(GPIO_BUTTON, GPIO_MODE_INPUT);
       gpio_set_pull_mode(GPIO_BUTTON, GPIO_PULLUP_ONLY);
   }

   // GPIO 控制
   gpio_set_level(GPIO_LED, 1);         // 高电平
   gpio_set_level(GPIO_LED, 0);         // 低电平

   int level = gpio_get_level(GPIO_BUTTON);  // 读取

   // 翻转
   gpio_set_level(GPIO_LED, !gpio_get_level(GPIO_LED));

NVS 非易失存储
===============

.. code-block:: c

   #include "nvs_flash.h"
   #include "nvs.h"

   void nvs_example(void)
   {
       // 初始化 NVS
       esp_err_t err = nvs_flash_init();
       if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
           err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
           ESP_ERROR_CHECK(nvs_flash_erase());
           err = nvs_flash_init();
       }

       nvs_handle_t handle;
       ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &handle));

       // 写入
       int32_t counter = 0;
       nvs_set_i32(handle, "counter", counter);
       nvs_commit(handle);

       // 读取
       nvs_get_i32(handle, "counter", &counter);

       nvs_close(handle);
   }

中断处理
=========

.. code-block:: c

   #include "driver/gpio.h"

   static QueueHandle_t gpio_evt_queue = NULL;

   // 中断处理函数 (在 ISR 上下文中执行)
   static void IRAM_ATTR gpio_isr_handler(void *arg)
   {
       uint32_t gpio_num = (uint32_t)arg;
       xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
   }

   void gpio_isr_init(void)
   {
       gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

       // 安装 GPIO ISR 服务
       gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

       // 配置中断触发
       gpio_set_intr_type(GPIO_BUTTON, GPIO_INTR_NEGEDGE);
       gpio_isr_handler_add(GPIO_BUTTON, gpio_isr_handler,
                           (void *)GPIO_BUTTON);
   }

   // 在任务中处理中断事件
   void gpio_task(void *arg)
   {
       uint32_t io_num;
       while (1) {
           if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
               ESP_LOGI(TAG, "GPIO[%d] 中断触发", io_num);
               // 处理按键逻辑
           }
       }
   }

定时器
=======

.. code-block:: c

   #include "esp_timer.h"

   void periodic_timer_callback(void *arg)
   {
       ESP_LOGI(TAG, "定时器触发");
   }

   void timer_init(void)
   {
       const esp_timer_create_args_t timer_args = {
           .callback = &periodic_timer_callback,
           .name = "periodic"
       };

       esp_timer_handle_t timer;
       ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer));

       // 每 1000 ms 触发一次
       ESP_ERROR_CHECK(esp_timer_start_periodic(timer, 1000000));
   }

看门狗
=======

.. code-block:: c

   #include "esp_task_wdt.h"

   void task_wdt_example(void)
   {
       // 在主循环中喂狗
       while (1) {
           esp_task_wdt_reset();
           vTaskDelay(1000 / portTICK_PERIOD_MS);
       }
   }

.. warning::

   ESP-IDF 默认启用了中断看门狗（Interrupt WDT）和任务看门狗（Task WDT）。
   如果任务阻塞超过 5 秒未让出 CPU，Task WDT 会触发复位。
   可在 menuconfig 中调整超时或禁用。

错误处理
=========

.. code-block:: c

   esp_err_t err;

   // 检查并处理错误
   err = gpio_set_level(GPIO_LED, 1);
   if (err != ESP_OK) {
       ESP_LOGE(TAG, "GPIO 操作失败: %s", esp_err_to_name(err));
       return err;
   }

   // 断言（调试用，Release 版可禁用）
   ESP_ERROR_CHECK(err);        // 出错时打印并 abort
   assert(ptr != NULL);          // 标准断言
