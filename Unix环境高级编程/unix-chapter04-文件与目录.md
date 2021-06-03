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

**块特殊文件**：提供对设备带缓冲的访问，每次访问以固定长度为单位进行。

**字符特殊文件**：提供对设备不带缓冲访问，访问长度可变，系统设备均为字符或块特殊文件

**FIFO**：用于进程间通信，命名管道。

**套接字**：进程间的网络或者非网络通信。

**符号链接**：这种类型文件指向另一个文件。

类型保存在`st_mode`中

POSIX允许IPC进程间通信对象说明为文件，包括消息队列、信号量、共享存储对象。

shell中\代表要在下一行继续键入命令，shell会用>提示。

## 设置用户ID和设置组ID

与一个进程相关联的ID有6个或更多。

实际用户ID和实际组ID，标识我们实际是谁，登录时取自口令文件登录项，通常会话期间不变，超级用户可更改。

有效用户ID和有效组ID及附属组ID，决定文件访问权限

保存的设置用户ID和设置用户组ID在执行一个程序时包含了有效用户ID和有效用户组ID的副本。setuid函数会使用。

通常，有效和实际相等。

每个文件有所有者及组所有者，stat结构中`st_uid`和`st_gid`指定。

执行一个程序文件，进程有效ID通常为实际ID，有效组ID为实际组ID，通过`st_mode`文件模式字，设置特殊标志，即有效ID设置为文件所有者用户ID，同样可设置另一位，有效组ID设置为文件组所有者ID。

设置用户ID或者设置用户组ID，使执行程序的实际用户ID拥有文件所有者ID权限。通过常量`S_ISUID`和`S_ISGID`测试。

## 文件访问权限

`st_mode`值包含了对文件的访问权限位，每个文件有9个访问位 -rwxrwxrwx。

三类访问权限（读、写、执行）的使用规则：

1、用名字打开文件，对名字中每个目录，包括可能隐含的当前工作目录 应当有执行权限x。对于目录的读权限和执行权限意义不同，读权限允许读目录，获得目录下的文件名列表。执行权限允许通过该目录。

2、读权限决定是否能打开现有文件进行读，与`O_RDONLY和O_RDWR`有关

3、写权限决定是否能打开现有文件进行写，与`O_WRONLY和O_RDWR`有关

4、`O_TRUNC`必须对该文件有写权限

5、目录中创建文件，必须对目录有写执行权。

6、删除现有文件，必须对目录有写执行权，文件本身不需读写权限。

7、exec函数执行某文件，必须有执行权，文件必须普通。

内核测试权限顺序：进程有效用户ID为0——有效用户ID为所有者ID——有效组ID或附属组ID之一为文件组ID——其他用户。 匹配一个即不再向下测试。

## 新文件和目录的所有权

新文件用户ID设置为进程的有效用户ID，组ID可以是进程的有效组ID或者它所在目录的组ID，取决于它所在目录的设置组ID是否被设置。

目录设置组ID位被设置，则新文件组ID继承目录组ID。

## 函数access和faccessat

open打开文件，进程希望内核按其实际用户ID和实际组ID测试访问能力，验证实际用户能否访问一个给定文件。（原为有效用户ID和有效组ID，可能为设置用户ID）。测试顺序将**有效**改成**实际**。

```c
#include <unistd.h>
int access(const char *pathname, int mode);
int faccessat(int fd, const char *pathname, int mode, int flag); //成0败-1
```

测试文件存在mode设置为`F_OK`，否则为`R_OK|W_OK|X_OK`。

faccessat函数在绝对路径下或者fd为`AT_FDCWD`的相对路径下 和 access相同。否则为相对fd的pathname路径

flag改变faccessat行为，`AT_EACCESS`访问检查用调用进程的有效用户ID和有效组ID。

## 函数umask

为进程设置文件模式创建屏蔽字

```c
#include <sys/stat.h>
mode_t umask(mode_t cmask); //返回：之前的文件模式创建屏蔽字
```

umask -S可以看到符号格式 umask xxx更改文件模式创建屏蔽字

## 函数chmod、fchmod和fchmodat

更改现有文件访问权限

```c
#include <sys/stat.h>
int chmod(const char *pathname, mode_t mode);
int fchmod(int fd, mode_t mode);
int fchmodat(int fd, const char *pathname, mode_t mode, int flag);
//成0败-1
```

chmod对指定文件，fchmod对打开文件，fchmodat在绝对路径下或者fd为`AT_FDCWD`的相对路径下 和 chmod相同，flag改变fchmodat行为，比如不跟随符号链接。

改变文件权限位，进程有效用户ID必须等于文件所有者ID或者进程拥有超级用户权限。

`S_ISUID`和`S_ISGID`为设置ID常量，保存正文常量`S_ISVTX`。组合常量(`S_IRWXU`,`S_IRWXG`,`S_IRWXO`)。

chmod函数更新的只是i节点最近一次被更改的时间。

chmod自动清除权限位：没有超级用户权限设置普通文件粘着位`S_ISVTX`，mode中粘着位自动被关闭；新创建文件的组ID不上调用进程所属的组，无超级用户权限进程，新文件组ID不等于进程有效组ID位或进程附属组ID中的一个，则设置组ID位自动被关闭。

## 粘着位

可执行文件的`S_ISVTX`这一位被设置，当该程序第一次被执行，终止时，程序正文部分（机器指令）的一个副本仍被保存在交换区，**使下次执行该程序时较快将其装载入内存**。

UNIX文件系统中文件数据块可能是随机存放，交换区被作为一个连续文件来处理，对于通用应用程序(文本编辑程序和C语言编译器)，常常设置它们所在文件的粘着位。交换区同时存放设置粘着位的文本数有限。

粘着的含义：系统再次自举前，文件正文部分总在交换区中，又称保存正文位。有了虚拟存储系统及快速文件系统，不再需要使用该技术了。

SUS允许对目录设置粘着位，对目录设置粘着位，只有对该目录具有写权限的用户并满足：拥有此文件|拥有此目录|是超级用户，才能删除或者重命名该目录下文件。

## 函数chown、fchown、fchownat和lchown

用于更改文件的用户ID和组ID，owner或group参数任意一个是-1，对应ID不变。

```c
#include <unistd.h>
int chown(const char *pathname, uid_t owner, gid_t group);
int fchown(int fd, uid_t owner, gid_t group);
int fchownat(int fd, const char *pathname, uid_t owner, gid_t group, int flag);
int lchown(const char *pathname, uid_t owner, gid_t group);
//成功返回0，失败返回-1；
```

除引用文件是符号链接以外，这4个函数的操作类似。符号链接下，lchown和fchown(设置`AT_SYMLINK_NOFOLLOW`)更改符号链接本身的所有者，而不是符号链接所指向文件的所有者

fchown改变fd参数指向的打开文件的所有者，不能用于改变符号链接所有者。

fchownat与chown或者lchown在两种情况下相同：pathname为绝对路径；fd为`AT_FDCWD`而pathname参数为相对路径。flag为`AT_SYMLINK_NOFOLLOW`，则等效lchown，清除了标志，则等效chown。

BSD系统规定超级用户才能更改文件所有者，system V则允许任一用户更改拥有文件所有者

`_POSIX_CHOWN_RESTRICTED`对指定文件生效时，则：只有超级用户进程能更改该文件的用户ID；进程拥有此文件，owner等于-1或文件用户ID，且group等于进程有效组ID或进程附属组ID之一，则一个非超级用户进程可以更改该文件的组ID。

函数由非超级用户进程调用，成功返回时，文件的设置用户ID位和设置组ID位都被清除。

## 文件长度

stat结构成员中`st_size`表示以字节为单位的文件的长度。对普通文件、目录文件和符号链接有意义。

普通文件，文件长度可以为0，即开始读就得到EOF提升；目录文件长度通常是某个数（16或512）的整倍数；符号链接文件长度为文件名中的实际字节数。

UNIX系统大多提供字段`st_blksize`和`st_blocks`，第一个适用于文件I/O的块长度，第二个是所分配的实际512字节块块数。 `st_blksize`用于读操作，读文件所需时间最少，为提高I/O效率，一次读、写块长度个字节。

### 文件中空洞

由设置偏移量超过文件尾端并写入某些数据后造成。

ls查看文件长度，du看磁盘使用，LINUX报告的块数取决于环境变量`POSIXLY_CORRECT`，设置了则报告1024字节块块数，否则是512字节块块数。

wc -c用于计算文件中字符数，如果使用cat复制空洞文件，则空洞数据字节会被0填满。

du -s查看的实际字节与ls命令报告长度不同原因在于，**文件系统使用若干块存放指向实际数据块的各个指针。**

## 文件截断

在文件尾端截去数据缩短文件。特例：文件长度截断为0，打开文件时使用`O_TRUNC`。

```c
#include <unistd.h>
int truncate(const char *pathname, off_t length);
int ftruncate(int fd, off_t length);
//成功0 失败-1
```

将现有文件长度截断为length，长度大于length部分不再访问，长度小于length增加文件长度，原尾端至新尾端之间数据读作0。(可能创建空洞)。

## 文件系统

UNIX文件系统的基本结构，i节点和指向i节点的目录项之间的区别

磁盘分成多个分区，每个分区包含一个文件系统，i节点是固定长度的记录项，包含有关文件大部分信息。

![](C:\Users\asuss\AppData\Roaming\marktext\images\2021-05-12-13-09-04-image.png)

每个i节点都有个链接计数，值为指向该i节点的目录项数，只有链接计数减小至0才可删除该文件（释放该文件占用的数据块）。所以解除文件链接并不总意味着释放该文件占用磁盘块，删除目录项函数为unlink而不是delete。 stat中`st_nlink`包含链接计数。**该链接为硬链接**。

![](C:\Users\asuss\AppData\Roaming\marktext\images\2021-05-12-13-09-41-image.png)

符号链接文件的实际内容，在数据块中，包含了该符号链接指向的文件的名字，文件类型为`S_IFLNK`

i节点包含了文件有关所有信息：文件类型、文件访问权限位、文件长度、指向文件数据块的指针。**stat结构中大多数信息取自i节点**。目录项只有两项重要数据：i节点编号和文件名。i节点编号的数据类型是`ino_t`。

目录项的i节点编号指向同一文件系统相应i节点，不能跨文件系统，所以ln命令不能跨越文件系统。

![](C:\Users\asuss\AppData\Roaming\marktext\images\2021-05-12-13-10-07-image.png)

不更换文件系统情况下为文件重命名，文件实际内容未移动，只是构造了一个指向现有i节点的新目录项，并删除老目录项，链接计数不变，mv操作过程就是如此。

目录文件的链接计数字段，目录项也有i节点，在父目录中每个子目录都使该父目录链接计数增加1。任何一个叶目录（不包含任何其他目录的目录）链接计数总是2，本身和.目录项。，子目录的..目录项会给父目录链接计数增加1。

## 函数link、linkat、unlink、unlinkat和remove

任何一个文件可以有多个目录项指向其i节点。创建一个指向现有文件的链接的方法。

```c
#include <unistd.h>
int link(const char *existingpath, const char *newpath);
int linkat(int efd, const char *existingpath, int nfd, const char *newpath, int flag);
//成功返回0，否则-1
```

创建新目录项，引用现有文件，如果newpath已经存在则返回出错。

linkat中efd和existingpath指定现有文件，nfd和newpath指定新路径名，相对路径通过fd计算，fd为`AT_FDCWD`则为当前目录，路径为绝对路径则忽略相应fd。flag为符号链接参数控制，`AT_SIMLINK_FOLLOW`为指向符号链接目标的链接 清除则指向符号链接本身的链接

创建新目录项和增加链接计数是**原子操作**

虽然允许实现支持跨越文件系统的链接，但大多数实现要求现有和新建的两个路径名在同一个文件系统中。仅限于超级用户可以实现支持创建指向一个目录的硬链接，**可能导致文件系统出现循环**，所以很多文件系统不允许对于目录的硬链接。

为删除现有的目录项

```c
#include <unistd.h>
int unlink(const char *pathname);
int unlinkat(int fd, const char *pathname, int flag);
//成功返回0否则-1
```

删除目录项，且对pathname所引用的文件链接计数-1。解除对文件的链接，需对包含该目录项的目录具有写和执行权限。若对该目录设置粘着位，则对该目录必须有写权限，并具备三个条件之一：拥有该文件，拥有该目录，超级用户权限。

链接计数为0，才可删除该文件内容。若有进程打开了该文件，内容也不能删除。所以关闭文件，内核首先检查打开该文件进程个数，再检查链接计数，都为0再删除文件内容。

unlinkat在pathname绝对路径忽略fd，fd为`AT_FDCWD`则为当前目录下的pathname，否则为fd下相对路径。 flag使调用进程可以改变unlinkat函数的默认行为，为`AT_REMOVEDIR`则unlinkat类似rmdir，清除flag标志则为unlink操作。

unlink常用于程序崩溃时清除程序创建的临时文件，只有进程关闭某打开文件或者终止时，文件的内容才会被删除，unlink不会删除打开文件的内容。

pathname为符号链接，unlink删除符号链接，给出符号链接名情况下，没有函数可以删除该链接所引用的文件。

超级用户调用unlink可以删除目录，通常用rmdir。

remove也可以解除对文件或目录的链接，对文件remove=unlink，目录remove=rmdir

```c
#include <stdio.h>
int remove(const char *pathname);
//成功0 失败-1
```

## 函数rename和renameat

文件或目录可以用rename或者renameat函数进行重命名。

```c
#include <stdio.h>
int rename(const char *oldname, const char *newname);
int renameat(int oldfd, const char *oldname, int newfd, const char *newname);
//成功0 失败-1
```

1、oldname指文件而非目录，则为文件或符号链接重命名，newname已经存在，则它不能引用一个目录（若引用即函数失败）。newname存在而非目录情况，则将该目录项删除再将oldname重命名为newname。更改目录需对oldname和newname所在目录有写权限。

2、oldname为目录，为该目录重命名，newname已经存在，则它必须引用一个目录，而且该目录应当是空目录（否则失败），若newname存在且为空目录，则将其删除，再将oldname重命名newname。当为目录重命名，newname不能包含oldname作为路径前缀。

3、oldname或newname引用符号链接，则处理符号链接本身，而非所引用文件。

4、不能对 . 和 .. 重命名，即oldname和newname最后部分不能为 . 和 .. 。

5、oldname和newname引用同一文件，函数不做更改并成功返回。

newname存在则调用进程需对它有写权限。进程对包含oldname和newname的目录具有写和执行权限。

## 符号链接

符号链接是对文件的间接指针。目的在于避开硬链接的一些限制：

硬链接通常要求链接和文件位于同一文件系统中；只有超级用户才能创建指向目录的硬链接（底层文件系统支持情况下）。

对符号链接以及它指向何种对象并无文件系统限制，任何用户都可以创建指向目录的符号链接。

符号链接一般用于将一个文件或整个目录结构移到系统中另一个位置。

符号链接可能在文件系统中引入循环。使用ftw降序遍历文件结构可能报错，linux的ftw和nftw记录所有看到的目录避免重复所以不报错。 符号链接使用unlink即可消除循环，硬链接很难消除。

符号链接可以链接到不存在的文件。 ln -s

## 创建和读取符号链接

```c
#include <unistd.h>
int symlink(const char *actualpath, const char *sympath);
int symlinkat(const char *actualpath, int fd, const char *sympath);
// 成功返回0 失败返回 -1
```

函数创建指向actualpath的新目录项sympath，创建符号链接不需要actualpath存在。且两者不需要在同一文件系统中。

symlinkat的sympath由fd计算，如果sympath为绝对路径或者fd为`AT_FDCWD`则symlinkat和symlink等价。

由于open跟随符号链接，打开符号链接和读链接的名字通过

```c
#include <unistd.h>
ssize_t readlink(const char *restrict pathname, char *restrict buf, size_t bufsize);
ssize_t readlinkat(int fd, const char *restrict pathname, char *restrict buf, size_t bufsize);
// 成功返回字节数， 出错 -1
```

组合了open read close操作，函数执行成功返回读入buf的字节数。

## 文件的时间

每个文件维护三个时间字段

`st_atim`**文件数据**最后访问时间

`st_mtim`**文件数据**最后修改时间

`st_ctim`**i节点状态**最后更改时间

## 函数futimens、utimensat和utimes

更改文件的访问和修改时间，futimens和utimensat指定了纳秒级精度的时间戳。

```c
#include <sys/stat.h>
int futimens(int fd, const struct timespec times[2]);
int utimensat(int fd, const char *path, const struct timespec times[2], int flag);
//成功返回0 失败-1
```

times数组第一个元素包含访问时间，第二元素包含修改时间。为日历时间

时间戳可以按4种方式之一进行指定。

1、times为空指针，访问时间和修改时间两者均为当前时间。

2、times指向两个timespec结构数组，任一`tv_nsec`字段为`UTIME_NOW`，相应时间戳为当前时间，忽略`tv_sec`

3、任一`tv_nsec`字段为`UTIME_OMIT`，相应时间戳不变

4、`tv_nsec`非上述两种情况，则相应时间戳设置为相应的两个字段值。

utimensat根据fd计算路径，flag则判断是否符号链接，默认跟随符号链接，修改文件时间为符号链接时间。

```c
#include <unistd.h>
int utimes(const char *pathname, const struct timeval times[2]);
//成功返回0 失败-1
```

不能对状态更新时间指定值，因为使用utimes字段会自动更新。

## 函数mkdir、mkdirat、rmdir

前两者创建目录，后者删除目录

```c
#include <sys/stat.h>
int mkdir(const char *pathname, mode_t mode);
int mkdirat(int fd, const char *pathname, mode_t mode);
//成功返回0， 失败返回-1
```

创建空目录，目录项 . 和 .. 自动创建，文件访问权限mode有进程文件模式创建屏蔽字修改。

对目录至少需要设置一个执行权限位。

```c
#include <unistd.h>
int rmdir(const char *pathname);
//成0败-1
```

rmdir用于删除空目录。目录链接计数为0则释放目录占用空间（无进程打开此目录）。

## 读目录

对目录有访问权限的任一用户都可以读该目录，为防止文件系统产生混乱，只有内核才能写目录。

```c
#include <dirent.h>
DIR *opendir(const char *pathname);
DIR *fdopendir(int fd);
//成功返回指针，出错返回NULL；
struct dirent *readdir(DIR *dp);
//成功返回指针，若在目录尾或出错 返回-1
void rewinddir(DIR *dp);
int closedir(DIR *dp);
//成功0失败-1
long telldir(DIR *dp) //返回与dp相关联的目录中当前位置
void seekdir(DIR *dp, long loc);
```

opendir和fdopendir返回的DIR指针由另外5个函数使用。目录中目录项顺序与实现有关，通常不按字母顺序排列。

## 函数chdir、fchdir、getcwd

进程有当前工作目录，为相对路径搜索起点，前两者用于更改当前工作目录

```c
#include <unistd.h>
int chdir(const char *pathname);
int fchdir(int fd);
//成功返回0 失败则返回-1
```

shell工作目录不随程序调用chdir改变，cd命令内建在shell中，内核为每个进程只保存指向该目录v节点的指针等目录本身信息，并不保存该目录完整路径名。

```c
#include <unistd.h>
char *getcwd(char *buf, size_t size);
//成功返回buf；出错返回NULL；
```

得到当前工作目录完整的绝对路径名。

## 设备特殊文件

`st_dev`和`st_rdev`，每个文件系统所在存储设备由主、次设备号表示。主设备号标识设备驱动程序，次设备号标识特定的子设备。

系统中与每个文件名关联的`st_dev`是文件系统的设备号，该文件系统包含了文件名及对应i节点。

只有字符特殊文件和块特殊文件才有`st_rdev`，包含了实际设备的设备号。
