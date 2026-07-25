=================
数组与字符串
=================

数组和字符串是 C 语言中最基础的数据集合形式。

一维数组
=========

定义与初始化
-------------

.. code-block:: c

   // 声明并初始化
   int arr[5] = {1, 2, 3, 4, 5};

   // 部分初始化（未指定元素默认为 0）
   int arr2[5] = {1, 2};        // {1, 2, 0, 0, 0}

   // 由编译器推断大小
   int arr3[] = {1, 2, 3, 4};   // 大小 = 4

   // 不初始化（值不确定）
   int arr4[10];

   // 全部初始化为 0
   int arr5[5] = {0};

访问与遍历
-----------

.. code-block:: c

   int arr[5] = {10, 20, 30, 40, 50};

   // 索引访问（从 0 开始）
   printf("%d\n", arr[0]);      // 10
   printf("%d\n", arr[2]);      // 30

   // 修改元素
   arr[1] = 25;

   // 遍历
   for (int i = 0; i < 5; i++) {
       printf("arr[%d] = %d\n", i, arr[i]);
   }

.. warning::

   C **不检查数组越界**！访问 ``arr[5]`` 或 ``arr[-1]`` 不会报编译错误，
   但会访问到内存中相邻位置的数据，产生未定义行为。

.. code-block:: c

   int arr[3] = {1, 2, 3};
   arr[3] = 100;     // 越界！可能破坏其他变量或导致崩溃

数组名与指针的关系
-------------------

.. code-block:: c

   int arr[5] = {1, 2, 3, 4, 5};

   // 数组名是首元素地址（常量指针）
   printf("%p\n", arr);         // 数组首地址
   printf("%p\n", &arr[0]);     // 同上
   printf("%d\n", *arr);        // 1（对首地址解引用）

   // 数组名不是指针变量，不能赋值
   // arr = &something;         // 编译错误！

获取数组长度
-------------

.. code-block:: c

   int arr[] = {2, 4, 6, 8, 10};

   // 方法：总字节数 ÷ 单个元素字节数
   int length = sizeof(arr) / sizeof(arr[0]);
   printf("数组长度: %d\n", length);     // 5

.. caution::

   当数组作为函数参数传递时，会退化为指针，``sizeof`` 不再有效：

   .. code-block:: c

      void print_len(int arr[]) {
          // arr 已退化为指针，sizeof(arr) = 4 或 8
          int len = sizeof(arr) / sizeof(arr[0]);  // 错误！
          printf("%d\n", len);    // 结果不是数组长度
      }

   正确的做法是将数组长度作为单独参数传入。

多维数组
=========

.. code-block:: c

   // 2×3 二维数组
   int matrix[2][3] = {
       {1, 2, 3},
       {4, 5, 6}
   };

   // 访问元素
   printf("%d\n", matrix[0][1]);   // 2
   printf("%d\n", matrix[1][2]);   // 6

   // 遍历
   for (int i = 0; i < 2; i++) {
       for (int j = 0; j < 3; j++) {
           printf("%d ", matrix[i][j]);
       }
       printf("\n");
   }

   // 内存布局：行优先存储
   // matrix[0][0] [0][1] [0][2] [1][0] [1][1] [1][2]

字符数组与字符串
=================

C 语言中字符串用 ``char`` 数组表示，以 ``\0`` (空字符) 结尾。

字符串定义
-----------

.. code-block:: c

   // 方式 1：字符数组（可修改）
   char str1[] = {'H', 'e', 'l', 'l', 'o', '\0'};
   char str2[] = "Hello";                // 自动添加 \0，大小 = 6

   // 方式 2：字符指针（只读，不可修改内容）
   char *str3 = "Hello";                 // 存储在只读区

   // 方式 3：指定大小的字符数组
   char str4[20] = "Hello";              // 剩余空间填充 \0

.. warning::

   ``char *str3 = "Hello"`` 指向字符串字面量，**不可修改**。
   ``str3[0] = 'h'`` 会导致运行时错误。

字符串操作函数
--------------

需要包含 ``<string.h>``：

.. code-block:: c

   #include <string.h>

   char dest[20] = "Hello";
   char src[] = " World";

   // 字符串长度（不含 \0）
   int len = strlen(dest);               // 5

   // 字符串拷贝
   strcpy(dest, "Hi");                   // dest = "Hi"

   // 字符串连接
   strcat(dest, " there");              // dest = "Hi there"

   // 字符串比较（相等返回 0）
   if (strcmp("abc", "abc") == 0) { ... }

   // 安全版本（VC10 推荐）
   strcpy_s(dest, sizeof(dest), "Hi");
   strcat_s(dest, sizeof(dest), " there");

字符串输入
-----------

.. code-block:: c

   char name[50];

   printf("请输入姓名: ");
   // scanf 以空白字符分隔，不能读取带空格的字符串
   scanf("%s", name);                    // 危险！可能越界

   // 安全读取
   scanf_s("%s", name, (unsigned)sizeof(name));

   // 或使用 gets_s（读取整行，含空格）
   gets_s(name, sizeof(name));

常见字符串处理
--------------

.. code-block:: c

   #include <stdio.h>
   #include <string.h>

   int main(void) {
       char str[100] = "Hello, C Language!";
       char copy[100];

       // 复制
       strcpy_s(copy, sizeof(copy), str);

       // 转大写
       for (int i = 0; str[i] != '\0'; i++) {
           if (str[i] >= 'a' && str[i] <= 'z') {
               str[i] -= 32;            // ASCII 码差 32
           }
       }
       printf("大写: %s\n", str);        // HELLO, C LANGUAGE!

       // 查找子串
       char *pos = strstr(str, "C");
       if (pos != NULL) {
           printf("找到位置: %d\n", pos - str);
       }

       // 分割字符串
       char text[] = "apple,banana,orange";
       char *token = strtok(text, ",");
       while (token != NULL) {
           printf("%s\n", token);
           token = strtok(NULL, ",");
       }

       return 0;
   }

.. tip::

   VC10 中许多传统字符串函数（``strcpy``、``strcat`` 等）会触发安全警告。
   使用 ``_s`` 后缀的安全版本（如 ``strcpy_s``）可消除警告并增强安全性。
   也可在源文件开头定义 ``#define _CRT_SECURE_NO_WARNINGS`` 禁用警告。
