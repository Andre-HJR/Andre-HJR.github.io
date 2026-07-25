================================
MicroPython 在 ESP32 上
================================

MicroPython 是 Python 3 的精简实现，可在微控制器上运行。

固件烧录
=========

.. code-block:: bash

   # 安装 esptool
   pip install esptool

   # 擦除 Flash
   esptool.py --chip esp32 --port COM3 erase_flash

   # 烧录 MicroPython 固件
   # 下载固件: https://micropython.org/download/ESP32_GENERIC/
   esptool.py --chip esp32 --port COM3 \
       write_flash -z 0x1000 esp32-20231005-v1.21.0.bin

   # 验证
   esptool.py --chip esp32 --port COM3 verify_flash \
       0x1000 esp32-20231005-v1.21.0.bin

REPL 交互
=========

.. code-block:: bash

   # 使用串口连接 (115200 baud)
   # Windows: PuTTY / MobaXterm
   #          也可用: picocom / screen

   picocom /dev/ttyUSB0 -b 115200    # Linux
   screen /dev/ttyUSB0 115200         # macOS

.. code-block:: python

   >>> print("Hello ESP32!")
   Hello ESP32!

   >>> import machine
   >>> machine.freq()
   240000000

   >>> import esp
   >>> esp.flash_size()
   4194304

基本示例
=========

LED 闪烁
---------

.. code-block:: python

   from machine import Pin
   import time

   led = Pin(2, Pin.OUT)

   while True:
       led.on()
       time.sleep(0.5)
       led.off()
       time.sleep(0.5)

按键输入
---------

.. code-block:: python

   from machine import Pin

   button = Pin(0, Pin.IN, Pin.PULL_UP)
   led = Pin(2, Pin.OUT)

   while True:
       if button.value() == 0:    # 按下（低电平）
           led.on()
       else:
           led.off()

PWM 呼吸灯
-----------

.. code-block:: python

   from machine import Pin, PWM
   import time

   pwm = PWM(Pin(2), freq=1000)

   duty = 0
   direction = 1

   while True:
       duty += direction * 10
       if duty >= 1023:
           direction = -1
       elif duty <= 0:
           direction = 1
       pwm.duty(duty)
       time.sleep_ms(5)

ADC 读取
---------

.. code-block:: python

   from machine import ADC, Pin
   import time

   adc = ADC(Pin(34))          # ADC1 通道
   adc.atten(ADC.ATTN_11DB)     # 0~3.3V

   while True:
       raw = adc.read()         # 0~4095
       voltage = raw / 4095 * 3.3
       print(f"ADC: {raw}, {voltage:.2f}V")
       time.sleep_ms(500)

I2C 总线
=========

.. code-block:: python

   from machine import I2C, Pin
   import time

   # 初始化 I2C (ESP32 默认: SCL=22, SDA=21)
   i2c = I2C(0, scl=Pin(22), sda=Pin(21), freq=400000)

   # 扫描设备
   devices = i2c.scan()
   print(f"I2C 设备: {[hex(d) for d in devices]}")

   # 读写 (以 MPU6050 为例)
   i2c.writeto(0x68, bytearray([0x6B, 0x00]))  # 唤醒
   data = i2c.readfrom_mem(0x68, 0x3B, 14)     # 读传感器数据

WiFi 连接
=========

.. code-block:: python

   import network
   import time

   def wifi_connect(ssid, password):
       wlan = network.WLAN(network.STA_IF)
       wlan.active(True)

       if not wlan.isconnected():
           print(f"连接 {ssid}...")
           wlan.connect(ssid, password)

           timeout = 30
           while not wlan.isconnected() and timeout > 0:
               time.sleep(1)
               timeout -= 1

       if wlan.isconnected():
           print(f"已连接, IP: {wlan.ifconfig()[0]}")
       else:
           print("连接失败")

   wifi_connect("MyWiFi", "password123")

Web 服务器
-----------

.. code-block:: python

   import socket

   def web_server():
       addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]
       s = socket.socket()
       s.bind(addr)
       s.listen(5)
       print(f"Web 服务器启动: {addr}")

       while True:
           conn, addr = s.accept()
           print(f"客户端连接: {addr}")

           request = conn.recv(1024)
           html = """<!DOCTYPE html>
           <html><head><title>ESP32</title></head><body>
           <h1>Hello from ESP32 MicroPython!</h1>
           </body></html>"""

           conn.send('HTTP/1.1 200 OK\n')
           conn.send('Content-Type: text/html\n')
           conn.send('Connection: close\n\n')
           conn.sendall(html)
           conn.close()

文件管理
=========

.. code-block:: python

   # 上传文件到 ESP32
   # 使用 ampy / rshell / Thonny

   # ampy (Adafruit MicroPython Tool)
   pip install adafruit-ampy
   ampy --port COM3 put main.py
   ampy --port COM3 ls /
   ampy --port COM3 get boot.py

.. code-block:: python

   # 在 REPL 中管理文件
   import os

   os.listdir('/')          # 列出文件
   os.mkdir('/lib')         # 创建目录
   os.remove('test.py')     # 删除文件
   os.stat('main.py')       # 文件信息

.. tip::

   MicroPython 在 ESP32 上的限制：

   - 内存有限：约 100 KB 可用堆空间
   - 浮点运算较慢（软件模拟）
   - 不支持所有 ESP-IDF 功能（如 BLE Mesh）
   - 适用于原型验证，量产建议使用 ESP-IDF
