=================
结构体与联合体
=================

C 语言允许将不同类型的数据组合成复合类型。

结构体（struct）
=================

结构体将多个不同类型的变量组合为一个整体。

定义与声明
-----------

.. code-block:: c

   // 结构体定义
   struct Student {
       int    id;           // 学号
       char   name[50];     // 姓名
       int    age;          // 年龄
       float  score;        // 成绩
   };                      // 注意分号！

   // 声明结构体变量
   struct Student stu1;
   struct Student stu2 = {1001, "Alice", 20, 95.5f};

   // 定义+声明合并
   struct Point {
       int x;
       int y;
   } p1, p2;

   // 匿名结构体（仅用于声明变量，不可复用）
   struct {
       int x;
       int y;
   } point;

访问成员
---------

.. code-block:: c

   struct Student stu = {1001, "Bob", 20, 88.5f};

   // 使用 . 运算符访问
   printf("ID: %d\n", stu.id);
   printf("Name: %s\n", stu.name);
   printf("Age: %d\n", stu.age);
   printf("Score: %.1f\n", stu.score);

   // 修改成员
   stu.score = 92.0f;

   // 整体赋值
   stu = (struct Student){1002, "Charlie", 21, 76.0f};

结构体指针
-----------

.. code-block:: c

   struct Student stu = {1001, "David", 22, 90.0f};
   struct Student *p = &stu;

   // 使用 -> 运算符访问（等价于 (*p).xxx）
   printf("Name: %s\n", p->name);        // 推荐写法
   printf("Age: %d\n", (*p).age);        // 等价写法

   // 结构体指针作为函数参数（传指针效率高）
   void print_student(const struct Student *s) {
       printf("ID=%d, Name=%s, Score=%.1f\n",
              s->id, s->name, s->score);
   }

.. tip::

   结构体较大时，**传指针比传值更高效** (避免复制整个结构体)。
   用 ``const`` 保护不修改的指针参数。

结构体嵌套
-----------

.. code-block:: c

   struct Date {
       int year;
       int month;
       int day;
   };

   struct Employee {
       int id;
       char name[50];
       struct Date hire_date;      // 嵌套结构体
       struct Date birth_date;
   };

   struct Employee emp = {
       2001, "Eve",
       {2020, 3, 15},              // 嵌套初始化
       {1990, 7, 22}
   };

   // 访问嵌套成员
   printf("入职年份: %d\n", emp.hire_date.year);

结构体大小与对齐
=================

.. code-block:: c

   #include <stddef.h>   // offsetof

   struct Sample1 {
       char   a;    // 1 byte
       int    b;    // 4 bytes
       char   c;    // 1 byte
   };

   /* 实际内存布局 (默认 4 字节对齐):
    * a (1) + padding(3) + b (4) + c (1) + padding(3) = 12 字节
    */
   printf("sizeof(Sample1) = %zu\n", sizeof(struct Sample1));

   /* 重新排列成员以减少填充 */
   struct Sample2 {
       int    b;    // 4 bytes
       char   a;    // 1 byte
       char   c;    // 1 byte
       // padding(2)
   };
   /* 大小: b(4) + a(1) + c(1) + padding(2) = 8 字节 */

VC10 中编译指示：

.. code-block:: c

   #pragma pack(push, 1)   // 1 字节对齐 (取消对齐优化)
   struct Packed {
       char a;
       int  b;              // 可能未对齐, 访问性能降低
   };
   #pragma pack(pop)

.. important::

   - 默认对齐到成员最大类型的倍数
   - 重新排列成员（从大到小）可以减少填充
   - ``#pragma pack`` 可控制对齐，但可能导致性能下降或运行错误

typedef 类型别名
=================

.. code-block:: c

   // 为结构体定义别名
   typedef struct {
       int x;
       int y;
   } Point;                // 现在可以直接用 Point 声明

   Point p1 = {10, 20};
   Point p2;               // 不用写 struct Point

   // 为函数指针定义别名
   typedef int (*CompareFn)(const void *, const void *);

   // 使用
   CompareFn cmp = &my_compare;

.. tip::

   使用 ``typedef`` 可以简化复杂类型声明，提高代码可读性。
   但不要过度使用，避免隐藏了重要类型信息。

联合体（union）
================

联合体的所有成员**共享同一块内存**，大小等于最大成员的大小。

.. code-block:: c

   union Data {
       int    i;
       float  f;
       char   str[20];
   };

   union Data data;

   data.i = 42;
   printf("i = %d\n", data.i);     // 42

   data.f = 3.14f;
   printf("f = %f\n", data.f);     // 3.14（覆盖了 i 的值）
   printf("i = %d\n", data.i);     // 不确定（已被覆盖）

   printf("sizeof(union Data) = %zu\n", sizeof(union Data));
   // 20（最大成员 char[20] 的大小）

联合体的典型用途
-----------------

.. code-block:: c
   :caption: 类型标记联合体（Tagged Union）

   #include <stdio.h>

   typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_STR } DataType;

   typedef struct {
       DataType type;          // 标记当前存储的类型
       union {
           int    i;
           float  f;
           char   *s;
       } value;
   } Variant;

   void print_variant(Variant v) {
       switch (v.type) {
           case TYPE_INT:
               printf("Integer: %d\n", v.value.i);
               break;
           case TYPE_FLOAT:
               printf("Float: %f\n", v.value.f);
               break;
           case TYPE_STR:
               printf("String: %s\n", v.value.s);
               break;
       }
   }

   int main(void) {
       Variant v1 = {TYPE_INT,   .value.i = 42};
       Variant v2 = {TYPE_FLOAT, .value.f = 3.14f};

       print_variant(v1);
       print_variant(v2);

       return 0;
   }

.. note::

   联合体常用于：
   - 节省内存（不同时使用的字段共享空间）
   - 解析网络协议或二进制文件格式
   - 类型擦除（类似 C++ 的 ``std::variant``）

位域（Bit Field）
=================

允许以比特位为单位定义结构体成员，常用于嵌入式硬件编程。

.. code-block:: c

   // 设备寄存器定义
   struct DeviceReg {
       unsigned int enabled  : 1;    // 1 bit
       unsigned int mode     : 2;    // 2 bits
       unsigned int status   : 3;    // 3 bits
       unsigned int reserved : 2;    // 2 bits
   };

   struct DeviceReg reg = {0};
   reg.enabled = 1;
   reg.mode    = 2;

   printf("sizeof = %zu\n", sizeof(reg));   // 4（占用 1 个 int）

.. caution::

   位域的行为因编译器而异（位序、对齐规则、跨越边界等），
   可移植性较差，VC10 与其他编译器可能存在差异。
