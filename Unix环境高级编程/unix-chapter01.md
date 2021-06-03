# UNIX基础知识

shell: 命令行解释器

## 文件和目录

stat, fstat返回包含所有文件属性的一个信息结构。

创建新目录时自动创建两个文件名 (.) 指向当前路径； (..) 指向上一级路径 最高层目录..=.

### 插曲：centos8下apue.h安装的坑

apue.3e下载解压

wget libbsd以及libbsd-devel两个rpm包再进行rpm安装

```shell
wget https://download-ib01.fedoraproject.org/pub/epel/8/Everything/x86_64/Packages/l/libbsd-0.9.1-4.el8.x86_64.rpm
wget https://download-ib01.fedoraproject.org/pub/epel/8/Everything/x86_64/Packages/l/libbsd-devel-0.9.1-4.el8.x86_64.rpm
```

```bash
rpm -ivh libbsd-0.9.1-4.el8.x86_64.rpm
rpm -ivh libbsd-devel-0.9.1-4.el8.x86_64.rpm
```

更改include/apue.h文件

```c
#include <error.h>
#include <sys/sysmacros.h>

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2
//-----------------
#include "error.c" /*在末尾添加该.c文件*/
#endif
```

更改filedir/devrdev.c文件（添加`#include <sys/sysmacros.h>`），stdio/buf.c文件

将apue.h以及error.c文件移动至/usr/include中

## 输入和输出

文件描述符：标准输入、输出以及标准错误

open、read、write、lseek、close均为不带缓冲的I/O，使用文件描述符

标准I/O为无缓冲I/O提供了缓冲接口，无需担心缓冲区选取的最佳大小 (fgets、printf)

## 程序和进程

每个进程有唯一的数字标识符，进程ID ，为非负整数

getpid返回pid_t型变量，转换为长整型变量，提高可移植性

进程控制的三个主要函数：fork， exec， waitpid

线程和线程ID：一个进程内线程共享同一地址空间，文件描述符，栈及进程相关属性；线程有ID，所属进程内有效。

## 出错处理

系统函数出错返回负值，errno即为具有特定信息的整型变量。

多线程环境中每个线程有局部的errno

errno两条规则：不出错errno值不清除；errno不会为0。

`strerror`函数将errnum(errno) 映射为出错消息字符串 返回char*

`perror`基于errno当前值，在标准错误上产生一条出错信息。

出错恢复：致命错误与非致命错误，fatal error不可恢复。

## 用户标识

用户ID，用户登录名确定时其ID也确定，不可被用户更改，0为根用户或超级用户。

组ID，各个成员间共享资源，用户组的概念 /etc/group

附属组ID，一个用户属于多至16个其他组。

存储用户ID和组ID只需4字节。

## 信号

通知进程发生了某种情况。

三种处理方式：忽略信号；按系统默认方式处理（终止等）；捕捉信号并提供自编函数。

终端有两种产生信号的方法：中断键（delete或者Ctrl+C）和退出键（Ctrl+\），中断当前运行进程，另一种产生信号的方法是调用kill函数。

## 时间值

日历时间：time_t存储，秒数累计值。

进程时间：CPU时间，clock_t存储，以时钟滴答计算，一秒为50、60或者100时钟滴答。

进程执行时间通过三个进程时间值度量：时钟时间（总进程相关），用户CPU时间（用户指令），系统CPU时间（内核），用time指令获取

## 系统调用与库函数

操作系统提供服务入口，此程序向内核请求服务，入口点被称为系统调用。malloc调用sbrk; printf调用write

特点：良好定义、数量有限、直接进入内核。 

应用角度看，系统调用可视为C函数。

应用程序既可以调用库函数 也可以调用系统调用。
