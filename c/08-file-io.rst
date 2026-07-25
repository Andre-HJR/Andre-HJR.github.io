=============
文件输入输出
=============

C 语言通过文件指针（``FILE *``）和标准库函数进行文件操作。

文件操作基础
=============

核心步骤
---------

.. code-block:: text

   1. 打开文件（fopen） → 获取 FILE *
   2. 读写文件（fprintf / fscanf / fread / fwrite）
   3. 关闭文件（fclose）

.. code-block:: c
   :caption: 文件操作模板

   #include <stdio.h>
   #include <stdlib.h>

   int main(void) {
       FILE *fp = fopen("test.txt", "w");
       if (fp == NULL) {
           printf("文件打开失败！\n");
           return 1;
       }

       // 读写操作...

       fclose(fp);       // 关闭文件
       return 0;
   }

.. important::

   **始终检查 ``fopen`` 的返回值！** 文件可能因为权限、磁盘满、路径错误等无法打开。

文件打开模式
=============

.. list-table::
   :header-rows: 1

   * - 模式
     - 含义
     - 文件存在
     - 文件不存在
   * - ``"r"``
     - 只读
     - 正常打开
     - 出错
   * - ``"w"``
     - 只写
     - 清空内容
     - 创建新文件
   * - ``"a"``
     - 追加
     - 从末尾写入
     - 创建新文件
   * - ``"r+"``
     - 读写
     - 正常打开
     - 出错
   * - ``"w+"``
     - 读写
     - 清空内容
     - 创建新文件
   * - ``"a+"``
     - 读追加
     - 从末尾写入
     - 创建新文件
   * - ``"rb"`` / ``"wb"``
     - 二进制模式
     - 见上
     - 见上

.. note::

   文本模式下，VC10 会将换行 ``\n`` 转换为 ``\r\n``（写入时）或反向转换（读取时）。
   二进制模式不作转换。处理非文本文件请用 ``"b"`` 模式。

文本文件读写
=============

fprintf / fscanf
-----------------

.. code-block:: c

   FILE *fp = fopen("students.txt", "w");
   if (fp != NULL) {
       fprintf(fp, "%d %s %.1f\n", 1001, "Alice", 95.5f);
       fprintf(fp, "%d %s %.1f\n", 1002, "Bob",   87.0f);
       fclose(fp);
   }

   // 读取
   fp = fopen("students.txt", "r");
   if (fp != NULL) {
       int id;
       char name[50];
       float score;

       while (fscanf(fp, "%d %s %f", &id, name, &score) == 3) {
           printf("ID=%d, Name=%s, Score=%.1f\n", id, name, score);
       }
       fclose(fp);
   }

fgets / fputs
---------------

.. code-block:: c

   // 写入字符串
   FILE *fp = fopen("data.txt", "w");
   fputs("第一行文本\n", fp);
   fputs("第二行文本\n", fp);
   fclose(fp);

   // 逐行读取
   char line[256];
   fp = fopen("data.txt", "r");
   while (fgets(line, sizeof(line), fp) != NULL) {
       // fgets 会保留换行符
       printf("读取: %s", line);
   }
   fclose(fp);

.. tip::

   ``fgets(buf, size, fp)`` 是**安全**的——最多读取 ``size - 1`` 个字符，
   自动添加 ``\0``。对比不安全的 ``gets(buf)``（无大小限制），
   应始终使用 ``fgets``。

fgetc / fputc
--------------

.. code-block:: c

   // 逐个字符复制文件
   FILE *src = fopen("source.txt", "r");
   FILE *dst = fopen("copy.txt", "w");

   if (src != NULL && dst != NULL) {
       int ch;    // 注意是 int 不是 char！
       while ((ch = fgetc(src)) != EOF) {
           fputc(ch, dst);
       }
       fclose(src);
       fclose(dst);
   }

.. warning::

   ``fgetc`` 的返回值是 ``int`` 而非 ``char``，因为 ``EOF`` 是 ``-1``，
   用 ``char`` 无法区分合法字符和文件结束。

二进制文件读写
===============

fread / fwrite
--------------

.. code-block:: c

   #include <stdio.h>

   typedef struct {
       int   id;
       char  name[20];
       float score;
   } Record;

   int main(void) {
       Record records[] = {
           {1001, "Alice", 95.5f},
           {1002, "Bob",   87.0f},
           {1003, "Eve",   92.3f}
       };
       int count = 3;

       // 写入二进制文件
       FILE *fp = fopen("records.dat", "wb");
       if (fp != NULL) {
           fwrite(records, sizeof(Record), count, fp);
           fclose(fp);
       }

       // 读取二进制文件
       Record buf[10];
       fp = fopen("records.dat", "rb");
       if (fp != NULL) {
           int n = fread(buf, sizeof(Record), 10, fp);
           for (int i = 0; i < n; i++) {
               printf("%d %s %.1f\n",
                      buf[i].id, buf[i].name, buf[i].score);
           }
           fclose(fp);
       }

       return 0;
   }

.. tip::

   ``fread`` / ``fwrite`` 的参数：
   ``fread(缓冲区, 单个元素大小, 元素个数, 文件指针)``
   返回实际读取/写入的元素个数。

文件定位
=========

.. code-block:: c

   #include <stdio.h>

   FILE *fp = fopen("data.bin", "rb");
   if (fp == NULL) return 1;

   // 获取当前读写位置（相对于文件开头）
   long pos = ftell(fp);
   printf("当前位置: %ld\n", pos);

   // 定位到文件开头偏移 10 字节处
   fseek(fp, 10, SEEK_SET);       // 从开头算起

   // 定位到文件末尾前 5 字节
   fseek(fp, -5, SEEK_END);       // 从末尾算起

   // 定位到当前位置后移 3 字节
   fseek(fp, 3, SEEK_CUR);        // 从当前位置算起

   // 获取文件大小
   fseek(fp, 0, SEEK_END);
   long file_size = ftell(fp);
   printf("文件大小: %ld 字节\n", file_size);

   // 重置到文件开头
   rewind(fp);                     // 等价于 fseek(fp, 0, SEEK_SET)

   fclose(fp);

.. list-table::
   :header-rows: 1

   * - 函数
     - 功能
     - 返回值
   * - ``ftell(fp)``
     - 获取当前偏移
     - ``long``，失败返回 ``-1L``
   * - ``fseek(fp, offset, origin)``
     - 设置文件位置
     - 成功 0，失败非 0
   * - ``rewind(fp)``
     - 重置到文件开头
     - 无返回值
   * - ``feof(fp)``
     - 检查是否文件尾
     - 非 0 表示到达文件尾

错误处理
=========

.. code-block:: c

   #include <stdio.h>
   #include <errno.h>
   #include <string.h>

   FILE *fp = fopen("nonexistent.txt", "r");
   if (fp == NULL) {
       // 使用全局变量 errno 和 perror
       printf("错误码: %d\n", errno);
       perror("fopen");                 // 输出: fopen: No such file or directory

       // 或使用 strerror
       printf("错误信息: %s\n", strerror(errno));

       return 1;
   }

   // 检查文件尾
   while (!feof(fp)) {
       int ch = fgetc(fp);
       if (ch != EOF) {
           putchar(ch);
       }
   }

VC10 文件操作注意事项
=======================

.. code-block:: c

   // 1. 文件路径中的反斜杠
   FILE *fp = fopen("C:\\temp\\data.txt", "r");  // 需要转义

   // 2. fopen_s 安全版本
   FILE *fp_safe;
   errno_t err = fopen_s(&fp_safe, "test.txt", "r");
   if (err != 0) {
       printf("打开失败，错误码: %d\n", err);
   }

   // 3. _CRT_SECURE_NO_WARNINGS
   // 若不想用 _s 版本，可在 stdafx.h 或项目属性中定义：
   // #define _CRT_SECURE_NO_WARNINGS

   // 4. 文件缓冲区设置
   FILE *fp2 = fopen("large.txt", "r");
   char buffer[8192];
   setvbuf(fp2, buffer, _IOFBF, sizeof(buffer));  // 设置缓冲区
   // _IOFBF: 全缓冲  _IOLBF: 行缓冲  _IONBF: 无缓冲

.. tip::

   VC10 项目属性中修改 ``配置属性 → C/C++ → 预处理器 → 预处理器定义``，
   添加 ``_CRT_SECURE_NO_WARNINGS`` 可全局禁用安全函数警告。
