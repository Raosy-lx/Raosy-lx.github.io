# UNIX标准及实现

标准化工作重要部分是对每种实现必须定义的各种限制进行说明。

## UNIX标准化

ISO C；IEEE POSIX可移植操作系统接口 POSIX.1包含了ISO C标准库函数；Single Unix Specification：POSIX.1的一个超集，扩展了功能；FIPS

## UNIX系统实现

标准只是接口的规范，实现包括：SVR4；4.4BSD；FreeBSD；Linux；Mac OS X；Solaris

## 限制

两种必需的限制：编译时限制（头文件中可定义）；运行时限制（进程调用函数获得限制值）

编译时限制（头文件）；与文件或目录无关的运行时限制（sysconf）；与文件或目录有关的运行时限制（pathconf 和 fpathconf）

```c
#include <unistd.h>
long sysconf(int name);
long pathconf(const char *pathname, int name);
long fpathconf(int fd, int name);
```

ISO C编译时限制均在头文件 `<limits.h>`中；POSIX定义了很多限制和常量；XSI限制对SUS

## 功能测试宏

```bash
cc -D_POSIX_C_SOURCE=200809L file.c
#define _POSIX_C_SOURCE 200809L
```

基本系统数据类型，与实现相关。

幻数的概念，例子：for循环过程i的上界，16即为幻数，通过宏定义常量len提高可读性
