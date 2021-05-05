# 文件和目录

文件系统特征及文件性质，stat函数及stat结构各成员函数，文件属性修改的各个函数，UNIX文件系统结构以及符号链接，对目录操作的各个函数。

## 函数stat、fstat、fstatat和lstat

```c
#include <sys/stat.h>
int stat(const char *restrict pathname, struct stat *restrict buf);
int fstat(int fd, struct stat *buf);
int lstat(const char *restrict pathname, struct stat *restrict buf);
int fstatat(int fd, const char *restrict pathname, struct stat *restrict buf, int flag)
//成功返回0，失败返回-1
```

lstat类似stat，对符号链接不返回引用文件信息，fstatat相对于当前打开目录fd的路径名返回文件统计信息，flag参数控制是否跟随一个符号链接，默认返回实际文件信息。

## 文件类型

文件类型：普通文件，目录文件，块特殊文件，字符特殊文件，FIFO，套接字，符号链接。

**普通文件**：包含某种形式数据，文本或二进制数据，内容解释由处理文件的应用程序进行。特例：二进制可执行文件遵循标准化格式，内核能确定程序文本和数据加载位置。

**目录文件**：包含其他文件名字及指向与这些文件有关信息的指针。只有内核可以直接写目录文件，读只需进程有读权限，更改目录需特定函数。

**快特殊文件**：提供对设备带缓冲的访问，每次访问以固定长度为单位进行。

**字符特殊文件**：提供对设备不带缓冲访问，访问长度可变，系统设备均为字符或块特殊文件

**FIFO**：用于进程间通信，命名管道。

**套接字**：进程间的网络或者非网络通信。

**符号链接**：这种类型文件指向另一个文件。

POSIX允许IPC进程间通信对象说明为文件，包括消息队列、信号量、共享存储对象。

shell中\代表要在下一行继续键入命令，shell会用>提示。

## 设置用户ID和设置组ID

与一个进程相关联的ID有6个或更多。

实际用户ID和实际组ID，标识我们实际是谁，登录时取自口令文件登录项，通常会话期间不变，超级用户可更改。

有效用户ID和有效组ID及附属组ID，决定文件访问权限

保存的设置用户ID和设置用户组ID在执行一个程序时包含了有效用户ID和有效用户组ID的副本。setuid函数会使用。

通常，有效和实际相等。

每个文件有所有者及组所有者，stat结构中`st_uid`和`st_gid`指定。
