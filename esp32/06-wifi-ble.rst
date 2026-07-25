================================
WiFi 与 BLE 通信
================================

WiFi Station 模式
===================

.. code-block:: c

   #include "esp_wifi.h"
   #include "esp_event.h"
   #include "nvs_flash.h"

   static void wifi_event_handler(void *arg, esp_event_base_t base,
                                  int32_t id, void *event_data)
   {
       if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
           esp_wifi_connect();
       } else if (base == WIFI_EVENT &&
                  id == WIFI_EVENT_STA_DISCONNECTED) {
           esp_wifi_connect();  // 自动重连
       } else if (base == IP_EVENT &&
                  id == IP_EVENT_STA_GOT_IP) {
           ip_event_got_ip_t *event = event_data;
           ESP_LOGI(TAG, "获取到 IP: " IPSTR,
                    IP2STR(&event->ip_info.ip));
       }
   }

   void wifi_init_sta(void)
   {
       nvs_flash_init();
       esp_netif_init();
       esp_event_loop_create_default();
       esp_netif_create_default_wifi_sta();

       wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
       esp_wifi_init(&cfg);

       esp_event_handler_instance_register(
           WIFI_EVENT, ESP_EVENT_ANY_ID,
           &wifi_event_handler, NULL, NULL);
       esp_event_handler_instance_register(
           IP_EVENT, IP_EVENT_STA_GOT_IP,
           &wifi_event_handler, NULL, NULL);

       wifi_config_t wifi_config = {
           .sta = {
               .ssid = "MyWiFi",
               .password = "password123",
               .threshold.authmode = WIFI_AUTH_WPA2_PSK,
           },
       };
       esp_wifi_set_mode(WIFI_MODE_STA);
       esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
       esp_wifi_start();
   }

WiFi AP 模式（软热点）
========================

.. code-block:: c

   void wifi_init_ap(void)
   {
       esp_netif_create_default_wifi_ap();

       wifi_config_t wifi_config = {
           .ap = {
               .ssid = "ESP32_AP",
               .ssid_len = strlen("ESP32_AP"),
               .password = "12345678",
               .max_connection = 4,
               .authmode = WIFI_AUTH_WPA_WPA2_PSK,
           },
       };

       esp_wifi_set_mode(WIFI_MODE_AP);
       esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
       esp_wifi_start();

       ESP_LOGI(TAG, "AP 启动: ESP32_AP");
   }

HTTP 客户端
============

.. code-block:: c

   #include "esp_http_client.h"

   void http_get_example(void)
   {
       esp_http_client_config_t config = {
           .url = "http://api.example.com/data",
           .timeout_ms = 5000,
       };

       esp_http_client_handle_t client = esp_http_client_init(&config);
       esp_err_t err = esp_http_client_perform(client);

       if (err == ESP_OK) {
           int status = esp_http_client_get_status_code(client);
           int len = esp_http_client_get_content_length(client);
           ESP_LOGI(TAG, "HTTP 状态: %d, 长度: %d", status, len);
       }

       esp_http_client_cleanup(client);
   }

MQTT 客户端
============

.. code-block:: c

   #include "mqtt_client.h"

   static void mqtt_event_handler(void *handler_args,
       esp_event_base_t base, int32_t event_id, void *event_data)
   {
       esp_mqtt_event_handle_t event = event_data;
       switch (event->event_id) {
           case MQTT_EVENT_CONNECTED:
               ESP_LOGI(TAG, "MQTT 已连接");
               esp_mqtt_client_subscribe(event->client,
                                         "/topic/test", 1);
               break;
           case MQTT_EVENT_DATA:
               printf("收到: %.*s => %.*s\n",
                     event->topic_len, event->topic,
                     event->data_len, event->data);
               break;
           default:
               break;
       }
   }

   void mqtt_app_start(void)
   {
       esp_mqtt_client_config_t mqtt_cfg = {
           .broker.address.uri = "mqtt://broker.emqx.io:1883",
           .broker.address.port = 1883,
       };

       esp_mqtt_client_handle_t client =
           esp_mqtt_client_init(&mqtt_cfg);
       esp_mqtt_client_register_event(client,
           ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
       esp_mqtt_client_start(client);

       // 发布消息
       esp_mqtt_client_publish(client, "/topic/test",
                              "Hello ESP32", 0, 1, 0);
   }

BLE 蓝牙
=========

.. code-block:: c

   // BLE Server 示例（基于 NimBLE 或 Bluedroid）

   #include "esp_bt.h"
   #include "esp_bt_main.h"
   #include "esp_gap_ble_api.h"
   #include "esp_gatts_api.h"

   #define GATTS_TAG            "GATTS_DEMO"
   #define PROFILE_NUM           1
   #define DEVICE_NAME           "ESP32_BLE"

   void ble_init(void)
   {
       ESP_ERROR_CHECK(esp_bt_controller_mem_release(
           ESP_BT_MODE_CLASSIC_BT));

       esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
       esp_bt_controller_init(&bt_cfg);
       esp_bt_controller_enable(ESP_BT_MODE_BTDM);

       esp_bluedroid_init();
       esp_bluedroid_enable();
   }

.. tip::

   ESP-IDF 支持两种 BLE 协议栈：

   - **Bluedroid**：功能完整，内存占用大（~1.5 MB），支持 Classic BT
   - **NimBLE**：轻量实现，内存占用小（~400 KB），仅 BLE

   在 menuconfig → ``Component config → Bluetooth`` 中切换。

BLE 扫描（MicroPython）
-------------------------

.. code-block:: python

   import bluetooth
   import time

   ble = bluetooth.BLE()
   ble.active(True)

   def bt_irq(event, data):
       if event == 5:  # _IRQ_SCAN_RESULT
           addr_type, addr, adv_type, rssi, adv_data = data
           name = ""
           # 解析广播数据中的名字
           print(f"发现设备: {addr.hex()}, RSSI: {rssi}")

   ble.irq(bt_irq)
   ble.gap_scan(30000, 30000, True)
