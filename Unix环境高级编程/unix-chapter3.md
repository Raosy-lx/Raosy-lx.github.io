# 文件I/O

open, read, write, lseek, close函数

不同缓冲长度对read, write的影响。

不带缓冲的I/O， 每个函数调用内核中的一个系统调用，非ISO C的组成。

多进程共享文件， dup, fcntl, sync, fsync, ioctl

## 文件描述符（file description，fd）

所有打开的文件都通过文件描述符 fd 引用，为非负整数。

打开现有文件或者创建新文件，内核向进程返回文件描述符。

open或者create返回fd，传递给read或者write。

fd中幻数0与进程标准输入关联，STDIN_FILENO；1与进程标准输出关联，STDOUT_FILENO；2与标准错误关联，STDERR_FILENO。包含在`<unistd.h>`

fd的变化范围`0 ~ OPEN_MAX-1`，早期UNIX上限19（一个进程最多打开20个文件），现在很多增至63。

## I/O函数

### open和openat

```c
#include <fcntl.h>
int open(const char *path, int oflag,.../*mode_t mode*/);
int openat(int fd, const char *path, int oflag,.../*mode_t mode*/);
```

path为打开或者创建的文件名， oflag为说明此函数的多个选项，open和openat函数返回的文件描述符一定是最小的未用描述符数值。

path为绝对路径 openat=open。

相对路径则 fd 指出了相对路径名开始地址，fd通过打开相对路径名所在目录获取；fd有特殊值`AT_FDCWD`，路径名从当前工作目录获取，openat在操作上与open类似。

openat的优势：使用相对路径名打开目录中文件，不再只能打开当前工作目录，对同一进程的多线程工作在不同目录下提供了实现；避免time-of-check-to-time-of-use(TOCTTOU)错误

TOCTTOU: 两个基于文件的函数调用，第二个调用依赖于第一个调用的结果，颠覆文件系统权限的小把戏，制造安全漏洞。

文件名与路径名截断。

### creat

```c
#include <fcntl.h>
int create(const char *path, mode_t mode);
//等效于 open(path, O_WRONLY|O_CREAT|O_TRUNC, mode);    只写打开的fd
```

### close

```c
#include <fcntl.h>
int close(int fd); //成功返回0，出错返回-1
```

关闭文件会释放进程加在该文件上所有记录锁；

进程终止，内核自动关闭所有打开文件，不显式close关闭打开文件。

### lseek

```c
#include <unistd.h>
off_t lseek(int fd, off_t offset, int whence);
```

每个打开文件均有当前文件偏移量，读写操作均从当前文件偏移量处开始，系统默认初始打开偏移量为0。

whence三个值：SEEK_SET 0偏移量设置为文件开始处offset字节；SEEK_CUR 1偏移量设置为当前值+offset字节；SEEK_END 2偏移量设置为文件长度+offset字节，后两可正可负。

对普通文件，文件当前偏移量必须非负。

lseek仅将当前文件偏移量记录在内核，不引起I/O操作，用于下次读写操作。

文件偏移量大于文件长度会构成空洞，位于文件中未写的字节均被读为0。文件空洞并不要求在磁盘占用存储区。文件尾端后的新写数据需分配磁盘块，尾端至开始之间部分不需要分配

### read

```c
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t nbytes); //读到的字节数，到文件尾返回0，错误返回-1
```

实际读字节数少于要求读字节数情况：

普通文件读到要求字节数前到达文件尾；

终端设备读时，通常一次最多一行；

网络读取缓冲机制导致；

管道或者FIFO读取；

面向记录的设备读时，一次最多返回一个记录；

信号中断但已经读部分数据量

void*表示通用指针，返回值 ssize_t 为带符号整数，size_t 为无符号值

### write

```c
#include <unistd.h>
ssize_t write(int fd, const void *buf, size_t nbytes); //返回已写字节数或者-1
```

返回值与参数nbytes值相同，否则出错；

磁盘已写满，或超过一个给定进程文件长度限制。

一次成功写之后，该文件偏移量增加实际写的字节数。

### 文件共享

UNIX系统支持不同进程间共享打开文件。

内核使用3种数据结构表示打开文件：

1、进程表中有记录项，记录项包含一张打开文件描述符表，该表中每条记录为文件描述符标志(与fd关联)以及指向文件表项的指针。

2、内核为每个打开文件维持一张文件表，表中包含文件状态标志，current offset，指向文件v节点表项的指针

3、每个打开文件有v节点结构，包含文件类型及对此文件进行各种操作函数的指针，大多数包含i节点，索引节点（文件所有者，长度，指向文件实际数据块在磁盘所在位置的指针）。

多个文件描述符项指向同一文件表项或者不同文件表项，fd用于一个进程的一个描述符，文件状态标志应用于指向该文件表项任何进程中所有描述符。

### 原子操作

追加到一个文件：用lseek多进程会出错，O_APPEND保证原子性，内核写前偏移量设置为文件尾端，不需lseek+write。

pread和pwrite，相当于lseek后的读写，无法中断定位与读写，不更新当前文件偏移量。

```c
#include <unistd.h>
ssize_t pread(int fd, void *buf, size_t nbytes, off_t offset);
ssize_t pwrite(int fd, const void *buf, size_t nbytes, off_t offset);
```

创建文件，open和creat用O_CREAT实现原子操作。

### 函数dup和dup2

复制现有文件描述符

```c
#include <unistd.h>
int dup(int fd);
int dup(int fd, int fd2);
```

dup返回的新文件描述符一定是当前可用文件描述符中最小值，**共享文件表项**。

dup2用fd2指定新描述符值。

dup(fd) = fcntl(fd, F_DUPFD, 0);

dup2(fd, fd2) = close(fd2); fcntl(fd, F_DUPFD, fd2); 原子操作 捆绑两个操作，不同errno

### 函数sync、fsync、fdatasync

内核写入缓存区再写入磁盘，延迟写，保证磁盘实际文件系统与缓冲区中内容的一致性。

```c
#include <unistd.h>
int fsync(int fd);
int fdatasync(int fd);

void sync(void);
```

sync只将修改过的块缓冲区写入队列，不等实际写磁盘操作结束。

fsync和fdatasync只对fd指定文件起作用，等写操作结束返回，fsync除了同步更新文件数据还更新文件属性。

### 函数fcntl

```c
#include <fcntl.h>
int fcntl(int fd, int cmd,.../* int arg */); //函数返回值依赖于cmd
```

改变打开文件的属性，第三个参数可以为整数也可以为指向结构的指针。

fcntl函数包含5种功能：

复制一个已有描述符 `cmd=F_DUPFD或F_DUPFD_CLOEXEC`

获取/设置文件描述符标志（close_on_exec）`cmd=F_GETFD或F_SETFD`

获取/设置文件状态标志`cmd=F_GETFL或F_SETFL`

获取/设置异步I/O所有权 `cmd=F_GETOWN或F_SETOWN`

获取/设置记录锁`cmd=F_GETLK或F_SETLK或F_SETLKW`

`F_DUPFD`复制文件描述符，尚未打开的各描述符中大于等于第三个参数值的各值的最小值。

`F_DUPFD_CLOEXEC`复制文件描述符，设置相关联的`FD_CLOEXEC`标志值，返回新fd。

`F_GETFD`返回对应fd的标志，当前只定义了一个fd标志`FD_CLOEXEC`，可能为0 系统默认 exec时不关闭，1 exec时关闭。

`F_SETFD`对于fd设置标志，值按照第三参数取整

`F_GETFL`返回对应fd的文件状态标志

`F_SETFL`文件状态标志设置为第三个参数取整

`F_GETOWN`获取接收SIGIO和SIGURG信号的进程ID或进程组ID

`F_SETOWN`设置接收SIGIO和SIGURG信号的进程ID或进程组ID，arg正为进程ID，负绝对值为进程组ID

`O_SYNC` UNIX系统write不使用，数据库系统需要使用，保证一致性。

### 函数ioctl

终端I/O使用多，是I/O操作杂物箱

```c
#include <unistd.h>
#include <sys/ioctl.h>
int ioctl(int fd, int request,...);
```

每个设备驱动程序可以定义自己专用ioctl命令，系统也提供通用ioctl命令。

### /dev/fd

目录，打开`/dev/fd/n`等效于复制描述符`n`。

`fd = open('/dev/fd/0', mode)`等效于`fd = dup(0)`且0为只读 mode为O_RDWR， fd也不能写。

Linux中/dev/fd把fd映射为指向底层物理文件的符号链接，为例外。

/dev/fd/1的create仍能工作。
