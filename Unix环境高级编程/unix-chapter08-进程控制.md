# 进程控制

UNIX进程控制，包括创建新进程、执行程序和进程终止。进程属性的各种ID，实际、有效和保存的用户ID和组ID，如何受到进程控制原语的影响。解释器文件和system函数。进程会计机制。

## 进程标识

每个进程有一个非负整数表示唯一进程ID。可用作其他标识符的一部分保证唯一性，应用程序把进程ID作为名字一部分来创建唯一文件名。

进程ID可以复用，即进程终止后，其进程ID成为复用候选者（同一时刻唯一）。大多数UNIX系统实现**延迟复用**算法，使新建进程的ID不同于最近终止的进程ID，防止将新进程误认为某个同ID已终止进程。

系统中的专用进程，ID为0进程通常是调度进程，又称交换进程。该进程为内核一部分，不执行任何磁盘程序，为系统进程。

 ID为1是init进程，自举过程结束时（自举程序从将操作系统内核从ROM加载入RAM）由内核调用。程序文件早期/etc/init，新版本/sbin/init。此进程负责自举内核后启动一个UNIX系统。init通常读取和系统有关的初始化文件，并将系统引导到一个状态。

init进程不会终止，是普通的用户进程，以超级用户特权运行。会成为所有孤儿进程的父进程

UNIX实现虚拟存储器中，某些使用进程ID 2 为页守护进程，来负责支持虚拟存储器系统的分页操作。

除了进程ID，每个进程还有其他标识符

```c
#include <unistd.h>
pid_t getpid(void); //返回调用进程的进程ID
pid_t getppid(void); //返回调用进程
uid_t getuid(void); //返回调用进程的实际用户ID
uid_t geteuid(void); //返回调用进程的有效用户ID
gid_t getgid(void); //返回调用进程的实际组ID
gid_t getegid(void); //返回调用进程的有效组ID
```

无出错返回。

## 函数fork

```c
#include <unistd.h>
pid_t fork(void);
//子进程返回0，父进程返回子进程ID;出错返回-1
```

fork创建的新进程为子进程，fork被调用一次，返回两次。子进程返回0，父进程返回子进程ID，原因：进程的子进程有多个，没有函数使一个进程可以获取其所有子进程ID。

fork使子进程返回0：一个进程只会有一个父进程，调用getppid获得父进程ID。进程ID 0总是由内核交换进程使用，子进程进程ID不可能为0。

子进程和父进程**继续执行fork调用之后的指令**，子进程是父进程的副本（获得父进程的数据空间、堆栈副本）。只是拥有副本并不共享存储空间，父进程和子进程共享正文段。

fork后通常跟随exec，很多实现并不执行父进程数据段、栈和堆的完全副本。使用写时复制。这些区域父子进程共享，内核将访问权限改变为只读，父子进程中任一试图修改这些区域，内核只为修改区域内存制作副本，通常是虚拟存储系统中的一页。

Linux支持clone系统调用，运行调用者控制父子进程共享部分。

FreeBSD提供rfork系统调用，类似clone。

Solaris提供两个线程库，一个用于POSIX，fork时创建进程只包含调用fork的线程。对于solaris线程，fork创建 进程包含调用线程所在进程的所有线程副本。 fork1与forkall。

一般来说，**fork后父子进程的执行顺序不确定**，取决于内核的调度算法。要求父子进程同步需要进程间通信。

写标准输出时，将buf长度-1为输出字节数，避免将终止null字符输出。strlen计算不包含终止null字节的字符串长度，sizeof包含null字节缓冲区长度。strlen使用一次函数调用，sizeof则是编译时计算缓冲区长度。

write函数无缓冲，标准I/O库带缓冲。fork，父进程数据空间复制到子进程中，缓冲区数据也被复制到子进程中。进程终止，缓冲区内容都被写到相应文件中。

### 文件共享

重定向父进程的标准输出时，子进程的标准输出也被重定向。fork，父进程打开的文件描述符都被复制到子进程，父子进程每个相同的打开描述符共享一个文件表项。

![](C:\Users\asuss\AppData\Roaming\marktext\images\2021-05-13-21-45-36-image.png)

进程具有三个不同打开文件，标准输入，标准输出，标准错误。**父子进程共享同一文件偏移量**，可以追加写入数据，不共享则交互困难。

父子进程写同一描述符指向的文件，不同步则输出会相互混合。

fork后处理fd有常见两种情况

1、父进程等待子进程完成，父进程无需对fd处理，子进程终止后，文件偏移量更新。

2、父进程和子进程各自执行不同的程序段。fork后，父子进程各自关闭不需使用的fd，这样不会干扰对方使用fd，网络服务进程常用。

除了打开文件，子进程还继承父进程的，实际用户和组ID，有效用户和组ID，附属组ID，进程组ID，会话ID，控制终端，设置用户ID和设置组ID，当前工作目录，根目录，文件模式创建屏蔽字，信号屏蔽和安排，close-on-exec标志，环境，连接的共享存储段，存储映像，资源限制。

父子进程的区别：

fork返回值不同，进程ID不同，进程父进程ID不同，子进程的`tms_utime,tms_stime,tms_cutime,tms_ustime`值为0

子进程不继承父进程设置的文件锁，子进程未处理闹钟被清除，子进程未处理信号集设为空

fork失败原因：系统中有太多进程；该实际用户ID的进程综述超过了系统限制，`CHILD_MAX`规定每个实际用户ID任一时刻可拥有的最大进程数。

fork两种用法：

1、一个父进程希望复制自己，同时执行不同代码段。网络服务进程中常见，父进程等待客户端服务请求。请求到达，父进程调用fork，子进程处理此请求，父进程继续等待。

2、一个进程执行不同程序，shell常见，fork返回后立刻调用exec。

fork+exec组合成 spawn。 两者分开，使子进程在fork和exec间可以更改自己的属性。

## 函数vfork

用于创建新进程，目的在于exec一个新程序，例shell。

vfork并不将父进程的地址空间完全复制到子进程，因为子进程立即调用exec或exit。

**子进程调用exec和exit前，在父进程空间运行**。无写时复制，所以子进程修改数据（除存放vfork返回值的变量）导致未知结果。

vfork保证子进程先运行，exec或者exit后父进程才能被调度。若两函数调用前依赖父进程进一步动作，可能导致死锁。

子进程得到父进程fd数组的副本。

## 函数exit

5种正常终止：

1、main函数内执行return，等效调用exit；

2、调用exit，包括调用各终止处理程序，关闭标准I/O流；

3、调用`_exit`和`_Exit`不冲洗标准I/O流，`_exit`由exit调用；

4、进程的最后一个线程在其启动例程中执行return语句，该线程返回值不用作进程返回值，进程以终止状态0返回；

5、进程的最后一个线程调用`pthread_exit`函数。

3种异常终止：

6、调用abort，产生SIGABRT信号；

7、进程接收到某些信号时，信号可由进程自身（abort）、其他进程或内核（进程引用地址空间外的存储单元或除数为0）产生；

8、最后一个线程对”取消“请求作出响应，默认情况下，”取消“以延迟方式发生：一个线程要求取消另一个线程，若干时间后目标线程终止。

进程终止最后会执行内核中的 同一段代码，为相应进程关闭所有打开描述符，释放它所使用的存储器等。

终止进程通知其父进程它如何终止的，exit三个函数通过**退出状态**作为参数传给函数。异常终止情况下，内核产生一个指示其异常终止原因的**终止状态**。

任一情况下，父进程可以使用wait或waitpid函数取得其**终止状态**。

`_exit`调用，内核将退出状态转换成终止状态。子进程正常终止，父进程可以获得子进程退出状态。

父进程在子进程之前终止，子进程的父进程将变成init进程，即被init进程收养。一个进程终止时，内核逐个检查所有活动进程，判断它是否是正要终止进程的子进程，是则更改父进程ID为1，保证进程均有父进程。

子进程在父进程之前终止，内核为每个终止子进程保存了一定量的信息，其父进程调用wait和waitpid时，可以得到这些信息。至少包括进程ID，该进程终止状态以及该进程使用的CPU时间总量。 内核释放终止进程的存储区，关闭其所有打开文件。

一个已经终止、但父进程尚未对其进行善后处理（获得子进程信息，释放占用资源）的进程被称为僵尸进程。 ps命令将僵尸进程状态打印为Z。

init被编写成无论何时有一个子进程终止，就调用wait获得其终止状态，防止系统中塞满僵尸进程。

## 函数wait和waitpid

一个进程正常或异常终止，内核向其父进程发送SIGCHLD信号。子进程终止是异步事件（父进程运行的任何时候发生），信号是内核向父进程发的异步通知。

父进程可以选择忽略该信号或提供该信号发生时被调用执行的函数（信号处理程序）。系统默认是忽略。

调用wait或waitpid的进程：

1、如果其所有子进程还在运行，则阻塞；

2、 一个子进程终止，正等父进程获取其终止状态，则取得该子进程终止状态立即返回；

3、没有任何子进程，立即出错返回。

```c
#include <sys/wait.h>
pid_t wait(int *statloc);
pid_t waitpid(pid_t pid, int *statloc, int options);
//成功返回进程ID；出错返回0或-1
```

子进程终止前，wait使其调用者阻塞，而waitpid可通过options使调用者不阻塞；waitpid并不等待在其调用之后的第一个终止子进程，可以控制它所等待的进程。

若子进程已经终止且是僵尸进程，wait立即返回并取得子进程状态；否则wait使其调用者阻塞直至一个子进程终止。wait返回子进程ID。

statloc是整型指针，若非空则指向单元存放终止进程的终止状态，不关心终止状态可指定为空指针。

函数返回状态，终止状态可通过wait.h中的宏来查看，WIFEXITED、WIFSIGNALED、WIFSTOPPED、WIFCONTINUED。

要等待一个指定的进程终止，早期调用wait，将其返回的进程ID和所期望ID比较，不是期望则保存ID和终止状态，再调用wait。

通过waitpid函数，pid==-1，等等任一子进程，waitpid=wait；pid>0，等待进程ID与pid相等子进程；pid==0，等待组ID等于调用进程组ID的任一子进程；pid<-1，等待组ID等于pid绝对值的任一子进程。

waitpid返回终止子进程的进程ID。wait出错是调用进程无子进程（函数调用被一个信号中断，可能返回另一种错）。waitpid若指定进程或进程组不存在，或pid指定进程不是调用进程的子进程，可能出错。

options参数使进一步控制waitpid操作。

waitpid提供wait外三个功能

1、waitpid等待特定进程

2、waitpid提供了wait的非阻塞版本

3、waitpid通过WUNTRACED和WCONTINUED支持作业控制。

## 函数waitid

```c
#include <sys/wait.h>
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);
//成功0 出错-1
```

waitid允许一个进程指定要等待的子进程，使用两单独参数表示子进程所属类型，而非将此与进程ID或进程组ID合成一个参数。

id参数与idtype相关，idtype包括`P_PID`, `P_PGID`, `P_ALL`。options指示调用者关注哪些状态变化。

infop为指向siginfo的指针，结构包含造成子进程状态改变有关信号的详细信息。

## 函数wait3和wait4

允许内核返回由终止进程及其所有子进程使用的资源概况

```c
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
pid_t wait3(int *statloc, int options, struct rusage *rusage);
pid_t wait4(pid_t pid, int *statloc, int options, struct rusage *rusage);
//成功返回进程ID，出错返回-1
```

资源统计信息包括用户CPU时间，系统CPU时间，缺页次数，接收到信号的次数等。 getrusage

## 竞争条件

多个进程对共享数据进行某种处理，最终结果取决进程运行顺序，发生了竞争条件。

fork后某逻辑显式或隐式地依赖于fork后父子进程运行顺序，则fork产生竞争条件。

调用sleep不能保证，系统负载很重情况下。

进程希望子进程终止，调用wait，进程等待父进程终止，循环判断其父进程是否ID为1。

`while(getppid() != 1) sleep(1);`

该形式循环称为轮询，浪费了CPU时间，调用者每隔1s被唤醒进行条件测试。

避免竞争条件和轮询，多个进程间需要某种形式的信号发送和接收方法。UNIX采用信号机制，各种形式的IPC进程间通信也可使用。

TELL和WAIT例程可以使父子进程按序执行。

## 函数exec

进程调用exec函数，该进程执行的程序完全替换为新程序，新程序从其main函数开始执行。exec并不创建新进程，进程ID不变。使用磁盘新程序替换当前进程正文段、数据段、堆栈

7种exec函数，fork创建新进程，exec初始执行新程序。exit和wait处理终止和等待终止。

```c
#include <unistd.h>
int execl(const char *pathname, const char *arg0, .../* (char *)0 */);
int execv(const char *pathname, char *const argv[]);
int execle(const char *pathname, const char *arg0, ...
        /* (char *)0, char *const envp[] */);
int execve(const char *pathname, char *const argv[], char *const envp[]);
int execlp(const char *filename, const char *arg0, .../* (char *)0 */);
int execvp(const char *filename, char *const argv[]);
int fexecve(int fd, char *const argv[], char *const envp[]);
//成功不返回，出错返回-1
```

**第一个区别**，前4函数路径名为参数，后两函数是文件名，最后是文件描述符。

filename参数包含/，则视为路径名；否则按PATH环境变量，在它所指定各目录搜寻可执行文件。

PATH包含了目录表（路径前缀），目录间用冒号分隔

`PATH=/bin:/usr/bin:/usr/local/bin:.`

零长前缀开始结尾均可用：表示，中间用：：表示。（存在零长的前缀）

execlp或execvp在路径前缀中的一个找到了并非连接编译器产生的机器可执行文件，认为该文件是shell脚本，调用/bin/sh，以该filename作为shell输入。

fexecve函数避免了寻找正确的可执行文件，而依赖调用进程。调用进程使用fd验证所需文件且无竞争地执行该文件。 否则，拥有特权的恶意用户可找到文件位置并验证，在调用进程执行文件前替换可执行文件。 TOCTTOU 检查到使用时间

**第二个区别**，参数表传递有关，execl、execlp、execle使用list，新程序每个命令行参数说明为一个单独的参数，参数表以空指针结尾。

execv、execve，execvp，fexecve，先构造一个指向各参数的指针数组，再将数组地址作为参数。

对于list，命令行参数通常

`char *arg0, char *arg1,...,char *argn, (char *)0`

若一个整型数长度与char*长度不同，exec函数的实际参数将出错。

**最后一个区别**，向新程序传递环境表相关，以e结尾的三个函数，可以传递指向环境字符串指针数组的指针。其他4个调用进程中environ变量为新程序复制现有环境。setenv和putenv不能影响父进程环境。

通常，进程允许将环境传播给子进程，有时会指定某一确定的环境。

每个系统对参数表和环境表的总长度都有个限制，ARG_MAX给出，值至少4096字节。

摆脱参数表长度限制，使用xargs，断长参数表成几部分。

新程序会从调用进程继承属性，对打开文件的处理与每个描述符的执行时关闭（`close-on-exec`）标志值有关，进程中每个打开的描述符都有一个执行时关闭标志。设置该标志则exec时关闭该描述符，否则仍打开该描述符。

用fcntl设置该执行时关闭标志，否则系统默认不设置，即exec后fd仍打开。

POSIX.1 要求exec时关闭，通过opendir函数实现，调用fcntl为对应打开的目录流描述符设置执行时关闭标志。

exec前后实际用户ID和实际组ID不变，有效ID取决于所执行程序文件的设置用户ID和设置组ID是否设置。

很多UNIX实现中，只有execve是内核系统调用。

![](C:\Users\asuss\AppData\Roaming\marktext\images\2021-05-15-14-31-03-image.png)

execlp和execvp使用PATH环境变量，fexecve库函数使用/proc把fd转换成路径名，execve执行程序。

## 更改用户ID和更改组ID

特权以及访问控制，基于用户ID和组ID。

设计应用时，我们总是试图使用最小特权模型。

```c
#include <unistd.h>
int setuid(uid_t uid);
int setgid(gid_t gid);
//成功返回0 出错返回-1
```

更改用户ID规则（适用于组ID）

1、进程拥有超级用户特权，setuid函数将实际用户ID、有效用户ID及保存的设置用户ID设置为uid。

2、进程没有超级用户特权，uid等于实际用户ID或保存的设置用户ID，setuid只将有效用户ID设置为uid，不更改实际用户ID和保存的设置用户ID。

3、上面两条件都不满足，errno设置为EPERM，返回-1

`_POSIX_SAVED_IDS`为真，若为假上述保存的设置用户ID部分无效。

内核所维护的3个用户ID。

1、只有超级用户进程可以更改实际用户ID，通常实际用户ID为用户登录由login程序设置。

2、仅当对程序文件设置了设置用户ID位，exec函数才设置有效用户ID，否则维持有效用户ID现有值。不能将有效用户ID设置为任一随机值。

3、保存的设置用户ID是由exec复制有效用户ID得到的，若设置了文件的设置用户ID位，则在exec根据文件的用户ID设置了进程的有效用户ID后，该副本被保存。

![](C:\Users\asuss\AppData\Roaming\marktext\images\2021-05-15-15-20-58-image.png)

getuid和geteuid只能获取实际用户ID和有效用户ID 当前值，不能获取保存的设置用户ID当前值。 getresuid和getresgid可以获取保存的设置UID和GID

### 函数setreuid和setregid

交换实际用户ID和有效用户ID的值（组）

```c
#include <unistd.h>
int setreuid(uid_t ruid, uid_t euid);
int setregid(gid_t rgid, gid_t egid);
//成功返回0，出错返回-1
```

其中任一参数为-1，表示相应ID应当保持不变。

非特权用户总能交换实际用户ID和有效用户ID。允许一个设置用户ID程序交换成用户的普通权限，而后又可再次交换回设置用户ID权限。

程序在子进程调用exec前，将子进程实际用户ID和有效用户ID都设置成普通用户ID。

### 函数seteuid和setegid

只更改有效用户ID和有效组ID

```c
#include <unistd.h>
int seteuid(uid_t uid);
int setegid(gid_t gid);
//成功返回0，出错返回-1
```

非特权用户可将其有效用户ID设置为实际用户ID或其保存的设置用户ID。 特权用户可将有效用户ID设置为uid。

![](C:\Users\asuss\AppData\Roaming\marktext\images\2021-05-15-15-35-41-image.png)

### 组ID

函数类似方式适用于各个组ID附属组ID不受setgid、setregid、setegid影响。

为防止被欺骗而运行不被允许的命令或读写没有访问权限的文件，at命令和最终代表用户运行命令的守护进程必须在两种特权间切换：用户特权和守护进程特权。

1、程序文件由root用户拥有，其设置用户ID位已经设置，运行程序时，实际用户ID为uid，euid和保存的设置用户ID为root

2、at程序降低特权，setuid吧euid设置为uid

3、at程序以uid特权运行，直到需要访问控制哪些命令即将运行，这些命令需要何时运行的配置文件时（由为用户运行命令的守护进程持有）。at调用setuid将有效用户IDeuid设置为root，设置为保存的设置用户ID。

4、修改文件从而记录将运行的命令及运行时间后，setuid，将euid设置为uid

5、守护进程以root特权运行，代表用户运行命令，守护进程调用fork，子进程调用setuid将用户ID更改至我们的用户ID，即所有ID均为uid。

## 解释器文件

文本文件，其起始行形式

`#! pathname [optional-argument]`

感叹号和pathname间空格可选，`! /bin/sh`

pathname通常是绝对路径名，无需特殊处理（使用PATH路径搜索）。识别文件由内核作为exec系统调用处理的一部分来完成。

内核使调用exec函数的进程实际执行的并不是解释器文件，而是该解释器文件第一行中pathname所指定文件。

解释器由解释器文件第一行pathname指定。

系统对解释器文件的第一行有长度限制。

解释器程序支持-f选项，则pathname后使用-f，`! /bin/awk -f` 此解释器文件中后跟随awk程序

识别解释器文件的是内核，所以解释器文件带来了内核的额外开销，但其有用在

1、有些程序用某种语言写的脚本，解释器文件可以隐藏该事实，awkexample不需要知道程序是awk脚本

2、解释器脚本在效率方面提供好处，若酱awk脚本放在shell脚本中。用一个shell脚本代替解释器脚本需要更多开销，fork，exec和wait。

3、解释器脚本允许使用除/bin/sh以外的其他shell来编写shell脚本。 当execlp找到一个非机器可执行的可执行文件，总是调用/bin/sh来解释，可用解释器脚本改写。

## 函数system

执行命令字符串，发出dos命令，fork子进程执行字符串对应命令

```c
#include <stdlib.h>
int system(const char *cmdstring);
```

cmdstring为空指针，仅当命令处理程序可用时，system返回非0，用于确定给定操作系统是否支持system。

system在实现中调用了fork，exec和waitpid，存在3种返回值

1、fork失败或waitpid返回出EINTR之外的出错，system返回-1，并设置errno指示错误类型

2、exec失败返回值如同exit(127)

3、都成功，返回shell终止状态。

使用system而不是直接使用fork和exec的优点：system进行了所需的各种出错处理以及各种信号处理。

`while((lastpid = wait(&status)) != pid && lastpid != -1);`

如果调用system的进程在调用它之前生成子进程，将引起问题，while语句一直循环执行直至由system产生的子进程终止，如果不是pid标识的任一子进程在pid前终止，进程ID和终止状态都被丢弃。

### 设置用户ID程序

在设置用户ID程序中调用system，fork和exec后仍被保持。若进程正以特殊权限（设置用户ID或设置组ID）运行，又想生成另一个进程执行另一个程序，应当使用fork和exec，且在fork后exec前更改回普通权限。 设置用户ID或设置组ID程序不允许调用system函数。

## 进程会计

启动进程会计处理选项，每当进程结束内核就写一个会计记录，典型会计记录包含总量较小的二进制数据，一般包括命令名、所使用CPU时间总量、用户ID和组ID、启动时间等。

acct函数启用和禁用进程会计。唯一使用该函数的是accton命令。超级用户执行带路径名参数的accton命令启用会计处理。会计记录写到指定文件中。

会计记录所需的各个数据由内核保存在进程表中，并在一个新进程被创建时初始化。进程终止写一个会计记录：

1、不能获取永不终止的进程的会计记录。 init和内核守护进程

2、会计文件中记录顺序对应于进程终止顺序，而非启动顺序。

会计记录对应于进程而非程序，fork后内核为子进程初始化一个记录而非exec新程序，exec不创建新会计记录，但更改命令名，AFORK标志被清除，如AexecB B exec C C exit，则命令名为C，CPU时间为A+B+C

`ac_stat`并非进程的真正终止状态。只是终止状态的一部分，异常终止，此字节只包含core标志位及信号编号数。

## 用户标识

任一进程可得到其实际用户ID和有效用户ID及组ID，可以调用`getpwuid(getuid())`找到运行该程序用户的登录名。一个用户可能多个登录名，对应同一个用户ID，系统通常记录用户登录时使用的名字。

```c
#include <unistd.h>
char *getlogin(void);
//成功返回指向登录名字符串的指针；出错返回NULL
```

调用此函数的进程没能连接到用户登录时所用终端，函数失败。 这些进程为守护进程。

给出登录名则getpwnam找用户记录

## 进程调度

内核决定调度策略和调度优先级，通过调整nice值选择更低优先级运行。只有特权进程允许提高调度权限。nice值越小，优先级越高

```c
#include <unistd.h>
int nice(int incr);
//成功返回新nice值NZERO，出错返回-1
```

incr被增加到调用进程的nice值上，incr太大降至最大合法值，太小也提高至最小合法值。调用nice前需清楚errno，在nice返回-1时检查值。 若nice返回-1 errno为0则调用成功。

```c
#include <sys/resource.h>
int getpriority(int which, id_t who);
//成功返回 -NZERO~NZERO-1间nice值；出错返回-1
```

getpriority获取一组相关进程的nice值

which参数`PRIO_PROCESS`进程`PRIO_PGRP`进程组`PRIO_USER`用户ID，who参数选择感兴趣的一个或多个进程。which参数作用于多进程，返回最小nice值。

```c
#include <sys/resource.h>
int setpriority(int which, id_t who, int value);
//成功返回 0；出错返回-1
```

value增加到NZER上成为新nice值。XSI要求进程调用exec保留nice值。

## 进程时间

墙上时钟时间、用户CPU时间、系统CPU时间

```c
#include <sys/times.h>
clock_t times(struct tms *buf);
//成功返回墙上时钟时间（时钟滴答为单位）；出错返回-1
```

```c
struct tms{
    clock_t tms_utime;
    clock_t tms_stime;
    clock_t tms_cutime;
    clock_t tms_cstime;
}
```

获得进程及已终止子进程的三个时间。

墙上时钟是相对值。`_SC_CLK_TCK`将返回的`clock_t`转换成秒数。
