# 进程间通信

进程间交换信息通过传送打开的文件，可经由fork或exec传送，也可通过文件系统来传送。

本章说明进程间通信的技术。

半双工管道，FIFO；全双工管道，命名全双工管道；XSI消息队列，XSI信号量，XSI共享存储；消息队列（实时），信号量，共享存储（实时）；套接字。

本章讨论 管道、FIFO、消息队列、信号量以及共享存储。

## 管道

两种局限性：

1、半双工（数据只能在一个方向上流动）。

2、管道只能在具有公共祖先的两个进程之间使用。通常一个管道由一个进程创建，在进程调用fork后，在父子进程之间使用。

FIFO没有2限制，UNIX域套接字没有1和2限制。

半双工管道是最常用的IPC形式。

```c
#include <unistd.h>
int pipe(int fd[2]);
//成功返回0 出错返回-1
```

创建管道，pipe返回两个fd，fd[0]为读而打开，fd[1]为写而打开。fd[1]的输出是fd[0]的输入。

fstat函数对管道的每一端都返回一个FIFO类型的fd。可用S_ISFIFO宏来测试管道。

单进程的pipe无用，通常进程**调用pipe再调用fork**，进而创建从父进程到子进程的IPC通道

fork后数据流方向，对于从父进程到子进程管道，父进程关闭管道读端fd[0]，子进程关闭管道写端fd[1]。子进程到父进程的管道，父进程关闭写，子进程关闭读。

管道一端被关闭：

1、读一个写端被关闭的管道，所有数据被读取后，read返回0。表示文件结束。（若管道写端还有进程，则不会产生文件的结束。可复制一个管道的描述符，使多个进程对其具有写打开的fd）

2、若写一个读端被关闭的管道，产生SIGPIPE。忽略该信号或者捕捉该信号并从其处理程序返回，则write返回-1，errno设置为EPIPE。

写管道（或FIFO），常量PIPE_BUF 规定了内核的管道缓冲区大小。对管道调用write，要求写的字节数小于内核缓冲区大小，则此操作不会与其他进程所写数据相互交叉。pathconf或fpathconf函数可以用于确定内核缓冲区大小。

## 函数popen和pclose

常见操作是创建一个连接到另一个进程的管道，然后读其输出或向其输入端发送数据。

```c
#include <stdio.h>
FILE *popen(const char *cmdstring, const char *type);
//成功返回文件指针；出错返回NULL。
int pclose(FILE *fp);
//成功返回cmdstring的终止状态；出错返回-1
```

两个函数实现的操作：创建一个管道，fork一个子进程，关闭未使用的管道端，执行一个shell运行命令，然后等待命令终止。

popen先执行fork，调用exec执行cmdstring，并返回一个标准I/O文件指针。type为r或者w指定了文件指针连接到的cmdstring的标准输出或者输入。指针可读或可写

pclose关闭标准I/O流，等待命令终止，返回shell的终止状态。shell不能被执行，则pclose返回与shell已执行exit(127)一样的终止状态。

popen不应由设置用户ID或设置组ID程序调用，执行命令时等同于：

`execl("/bin/sh", "sh", "-c", command, NULL);`

恶意用户可以越权调用。

标准输出通常行缓冲，提示并不包含换行符，写了提示后，需调用fflush

## 协同进程

UNIX系统过滤程序从标准输入读取数据，向标准输出写数据。过滤程序通常在shell管道中线性连接。一个过滤程序既产生某个过滤程序的输入，又读取该过滤程序的输出时，就变成了**协同进程**。

协同进程通常在shell后台运行，其标准输入和标准输出通过管道连接到另一个程序。初始化一个协同进程，将其输入和输出连接到另一个进程的shell语法很奇特。

popen只提供连接到另一个进程的标准输入或标准输出的一个单向管道，而协同进程则有连接到另一个进程的两个单向管道。将数据写到标准输入，经处理再从标准输出读取数据。

## FIFO

被称为命名管道。未命名的管道只能在两个相关的进程间使用，有一个共同的创建它们的祖先进程。FIFO为文件类型，stat的`st_mode`成员可知，用`S_ISFIFO`宏测试。

```c
#include <sys/stat.h>
int mkfifo(const char *path, mode_t mode);
int mkfifoat(int fd, const char *path, mode_t mode);
//成功返回0 出错返回-1
```

FIFO路径名存在于文件系统。创建FIFO类似于创建文件。

mode与open函数的mode参数规格说明相同。

mkfifoat函数与mkfifo函数相似，mkfifoat可被用于在fd表示的目录相关的位置创建一个FIFO：

1、如果path参数指定的是绝对路径名，fd参数被忽略，且mkfifoat和mkfifo行为类似。

2、如果path参数指定的是相对路径名，fd参数是一个打开目录的有效文件描述符，路径名和目录有关。

3、如果path参数指定的是相对路径名，fd参数有特殊值`AT_FDCWD`，路径名以当前目录开始，mkfifoat和mkfifo类似。

创建FIFO时，需要open打开。正常的文件I/O函数都需要FIFO。

open一个FIFO，非阻塞标志（`O_NOBLOCK`）的影响：

1、一般情况下，未指定非阻塞标志，只读open要阻塞到某个其他进程为写而打开这个FIFO为止。只写open要阻塞到某个其他进程为读而打开它为止。

2、指定非阻塞标志，只读open立即返回，若没有进程为读而打开一个FIFO，只写open返回-1，并将errno设置成ENXIO。

若write一个尚无进程为读而打开的FIFO，产生SIGPIPE。

若某个FIFO的最后一个写进程关闭该FIFO，则为该FIFO读进程产生一个文件结束标志。

**一个给定FIFO有多个写进程**，不希望多个进程所写数据交叉，则必须考虑原子写操作。`PIPE_BUF`指定了可被原子地写到FIFO的最大数据量。

FIFO的两种用途：

1、shell使用FIFO将数据从一条管道传送至另一条，无需创建中间临时文件。

2、客户进程-服务器进程应用程序中，FIFO用作汇聚点，在客户进程和服务器进程二者之间传递数据。

```c
mkfifo fifo1
prog3 < fifo1 &
prog1 < infile | tee fifo1 | prog2
```

![](C:\Users\asuss\AppData\Roaming\marktext\images\2021-06-15-13-46-54-image.png)

客户进程-服务器进程通信。

![](C:\Users\asuss\AppData\Roaming\marktext\images\2021-06-15-13-50-17-image.png)

## XSI IPC

3种IPC：消息队列、信号量以及共享存储器，其相似特征。

### 标识符和键

每个**内核**中IPC结构都用一个**非负整数的标识符**加以引用。如消息队列的使用只需知道其队列标识符。与fd不同，IPC标识符不是小的整数。IPC结构被创建，又被删除时，与这结构相关的标识符连续加1，直至达到一个整型数的最大正值，然后回到0。

标识符是IPC对象的内部名。为使多个合作进程能够在同一IPC对象汇聚，需提供外部命名方案，即**IPC对象与键关联**，键作为该对象的外部名。

创建IPC结构，都应指定一个键，数据类型`key_t`，通常为长整型，这个键由内核变换为标识符。

有多种方法使客户进程和服务器进程在同一IPC结构上汇聚：

1、服务器进程指定键`IPC_PRIVATE`创建一个新的IPC结构，将返回的标识符存放在某处（如一个文件）以便客户进程取用。此键保证服务器进程创建一个新的IPC结构。缺点在于：文件系统操作需要服务器进程将整型标识符写到文件中，此后客户进程读文件取标识符。

`IPC_PRIVATE`也可用于父进程子关系，父进程指定此键创建IPC结构，返回的标识符供fork后的子进程使用，接着，子进程将此标识符作为exec函数的一个参数传给一个新程序。

2、可在公用头文件中定义一个客户进程和服务器进程都认可的键。服务器进程指定此键创建一个新IPC结构。存在问题：该键已与一个IPC结构结合，此情况下get(msgget, semget, shmget) 出错返回。服务器进程必须处理该错误，删除已有IPC结构再试着创建它。

3、客户进程和服务器进程认同一个路径名和项目ID（0~255间字符值）。调用函数ftok将两个值变换为一个键，在方法2中使用此键。

```c
#include <sys/ipc.h>
key_t ftok(const char *path, int fd);
//成功返回键，出错返回(key_t)-1
```

path必须引用现有文件。产生键时，只使用id参数的低8位。

ftok创建的键通常用下列方式构成：path取得其stat结构中的`st_dev`和`st_ino`字段，将其与项目ID组合。两个路径名引用两个不同的文件，则ftok通常为这两个路径名返回不同的键。但因为i节点编号和键通常都存放在长整型中，所以创建键时可能会丢失信息，对于不同文件的两个路径名，如果项目ID相同，可能产生相同键。

3个get函数有两类似参数：key和整型flag。创建新的IPC结构时（通常服务器进程创建），若key是PRIVATE或以当前某类型IPC结构无关，flag需指定`IPC_CREAT`标志位。为引用现有队列（通常客户进程创建），key必须等于队列创建时指明的key值，且必须不知明CREAT

不能用PRIVATE作为键来引用现有队列，引用其创建的队列需要知道相关标识符。

创建新IPC结构，确保没有引用具有同一标识符的一个现有IPC结构，则必须在flag中同时指定`IPC_CREAT`和`IPC_EXCL`位。若IPC结构存在返回EEXIST。

### 权限结构

每个IPC结构关联一个`ipc_perm`结构。规定了权限和所有者，至少包括

```c
struct ipc_perm{
    uid_t uid; /* owner's effective user id */
    gid_t gid; /* owner's effective group id */
    uid_t cuid; /* creator's effective user id */
    gid_t cgid; /* creator's effective group id */
    mode_t mode; /* access modes */
    ...
};
```

创建IPC结构时，所有字段赋初值。调用ctl函数修改uid，gid和mode字段。修改字段的调用进程必须是IPC结构的创建者或超级用户。修改字段类似于对文件的chown和chmod

mode字段值，对于任何IPC结构不存在执行权限。消息队列和共享存储使用术语“读”和 “写”。信号量使用“读” 和 “更改”

用户读写对应 0400与 0200， 组读写对应 0040与 0020，其他读写对应 0004和0002

### 结构限制

3种形式的XSI IPC都有内置限制。大多数限制可通过重新配置内核改变，对IPC描述，需指出限制。sysctl观察修改内核配置参数，Solaris用prctl更改IPC限制。

### 优点和缺点

存在问题：

1、IPC结构在系统范围内起作用，没有引用计数。

如果进程创建消息队列，放入消息，而后终止，该消息队列及其内容不会被删除。直到：某进程调用msgrcv或msgctl读消息或删除消息队列；某个进程执行ipcrm命令删除消息队列；正在自举的系统删除消息队列。

将此与管道相比，最后一个引用管道的进程终止时，管道就被完全地删除了。FIFO而言，最后一个引用FIFO的进程终止，FIFO名字保留在系统，但留着FIFO数据被删除。

2、这些IPC结构在文件系统中没有名字。

不能用ls命令查看IPC对象，rm命令删除，chmod修改访问权限。需要内核增加全新的系统调用支持IPC对象，添加新命令 ipcs 和 ipcrm。

IPC不使用fd，不能使用多路转接I/O函数（select和poll）。很难一次使用一个以上IPC结构，或在文件或设备I/O中使用。

消息队列优点：可靠、流控制面向记录；以用非先进先出次序处理。

并非无连接，消息队列需某种技术获取队列标识符。被限制在一台主机，可靠。流控制在于：系统资源短缺，若接收进程不能再接收更多消息，发送进程需休眠，流控制条件消失，发送进程自动唤醒。

## 消息队列

**消息的链接表，存储在内核中**，由消息队列标识符标识。简称**队列**，标识符简称**队列ID**。

msgget用于创建一个新队列或打开一个现有队列。msgsnd将新消息添加到队列尾端。每个消息包含一个正长整型字段、一个非负长度以及实际数据字节数（对应于长度），所有这些将消息添加到队列时，传送给msgsnd。msgrcv用于从队列中取消息。并不是一定先进先出次序取消息，也可以按消息的类型字段取消息。

每个队列有个`msqid_ds`结构与其相关联：

```c
struct msqid_ds{
    struct ipc_perm   msg_perm; /* 权限结构 */
    msgqnum_t         msg_qnum; /* # of message on queue */
    msglen_t          msg_qbytes; /* max # of bytes on queue */
    pid_t             msg_lspid; /* pid of last msgsnd() */
    pid_t             msg_lrpid; /* pid of last msgrcv() */
    time_t            msg_stime; /* last-msgsnd() time */
    time_t            msg_rtime; /* last-msgrcv() time */
    time_t            msg_ctime; /* last-change time */
    ...
};
```

定义了队列当前状态。其限制包括 最大消息数，最大消息队列数，可发送的最长消息的字节数，一个特定队列的最大字节数。

```c
#include <sys/msg.h>
int msgget(key_t key, int flag);
//成功返回消息队列ID；出错返回-1
```

打开一个现有队列或创建一个新队列。

创建新队列需初始化`msqid_ds`结构的成员：

`ipc_perm`结构按权限结构初始化，qnum、lspid、lrpid、stime、rtime设置为0，ctime设置为当前时间，qbytes设置为系统限制值。

执行成功则msgget返回非负队列ID。此后该值被用于其他3个消息队列函数。

msgctl对队列执行多种操作。类似ioctl函数

```c
#include <sys/msg.h>
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
//成功返回0；出错返回-1
```

cmd指定对msqid指定的队列要执行的命令：

`IPC_STAT`取此队列的`msqid_ds`结构，将其存放在buf指向的结构中

`IPC_SET`将字段`msg_perm`结构中字段uid、gid、mode和`msg_qbytes`从buf指向的结构复制到队列相关的`msqid_ds`结构中。此命令只能由两种进程执行：有效用户ID等于`msg_perm`结构中字段cuid或uid，另一种具有超级用户特权进程。只有超级用户才能增加`msg_qbytes`的值

`IPC_RMID`从系统中删除该消息队列以及仍在该队列中所有数据。删除立即生效。对此队列后续操作，得到EIDRM错误。此命令只能由两种进程执行：有效用户ID等于`msg_perm`结构中字段cuid或uid，另一种具有超级用户特权进程。

这三条命令可用于信号量和共享存储。

```c
#include <sys/msg.h>
int msgsnd(int msqid, const void *ptr, size_t nbytes, int flag);
//成功返回0；出错返回-1
```

消息由一个正的长整型类型字段，一个非负长度(nbytes)以及实际数据字节数（对应于长度）。消息总是放在队列尾端。

ptr指向一个长整型数，包含正的整型消息类型，其后紧接着是消息数据（若nbytes是0，则无消息数据）。若发送的最长消息是512字节的，可定义下列结构：

```c
struct mymesg{
    long mtype;      /* positive message type */
    char mtext[512]; /* message date, of length nbytes */
};
```

ptr是指向mymesg结构的指针。接受者可使用消息类型以非先进先出次序取消息。

参数flag可指定为`IPC_NOWAIT`。类似于非阻塞I/O标志。

若消息队列已满（队列中消息总数上限或队列中字节总数达上限），指定NOWAIT使得msgsnd立即出错返回EAGAIN。没指定则进程会一直阻塞到：有空间容纳消息；或从系统删除此队列；或捕捉到一个信号并从信号处理程序返回。系统删除队列返回EIDRM，第三种返回EINTR。

删除消息队列处理并不完善。因为消息队列没有维护引用计数器，在队列被删除以后，仍在使用这一队列的进程下次使用会出错返回。信号量也以同样方式处理其删除。删除文件需等到使用该文件最后一个进程关闭它的fd，才能删除文件的内容。

msgsnd返回成功时，消息队列相关的msqid_ds结构会更新，lspid，stime以及qnum更新

```c
#include <sys/msg.h>
ssize_t msgrcv(int msqid, void *ptr, size_t nbytes, long type, int flag);
//成功返回消息数据部分的长度；出错返回-1
```

从队列中取用消息。ptr指向长整型数，后跟随存储实际消息数据的缓冲区。nbytes指定数据缓冲区长度。若返回消息长度大于nbytes，flag为`MSG_NOERROR`则消息被截断。否则返回E2BIG错误，消息仍在队列中。

参数type指定了想要的消息种类。

type==0 返回队列中第一个消息。

type>0 返回消息类型为type的第一个消息。

type<0 返回队列中消息类型值小于等于type绝对值的消息，若有若干个，取类型值最小的消息。

非0用于非先进先出次序读消息。type可以是优先权值，也可以是客户进程的进程ID。

flag指定为`IPC_NOWAIT`，若没有指定类型的消息可用，msgrcv返回-1，errno设置为ENOMSG。否则进程一直阻塞到有指定类型的消息可用，或从系统中删除此队列（返回-1，errno设置为EIDRM），或捕捉到一个信号并从信号处理程序返回（返回-1, errno为EINTR）

msgrcv成功执行，内核会更新与该消息队列相关联的`msqid_ds`结构，指示lrpid，rtime，并指示qnum减少一个。

消息队列与全双工管道速度方面已经没啥差别。尽可能不用。

## 信号量

计数器，用于为多个进程提供对共享数据对象的访问。

为了获取共享资源，进程需要执行的操作：

1、测试控制该资源的信号量。

2、若此信号量值为正，进程可使用该资源，此情况进程将信号量值减少1，表示它使用一个资源单位

3、否则，若此信号量为0，进程进入休眠状态，直至信号量值大于0。进程被唤醒后，返回至步骤1。

当进程不再使用由一个信号量控制的共享资源时，该信号量增1。若有进程正休眠等待，则唤醒。

信号量值的测试及减1是原子操作。信号量通常在**内核**中实现。

常用信号量形式被称为**二元信号量**。控制单个资源，其初始值为1。一般信号量初值为任意正值，表明有多少个共享资源单位。

XSI 信号量 ，其具有三种复杂的特性：

1、信号量并非是单个非负值，必需定义为含有一个或多个信号量值的集合。当创建信号量时，要指定集合中信号量值的数量。

2、信号量的创建独立于初始化。不能原子地创建一个信号量集合，并且对该集合中各信号量值赋初值。

3、即使没有进程正在使用各种形式的XSI IPC，它们仍然存在。有的进程终止时并没有释放已经分配给它的信号量，需要undo。

内核为每个信号量维护`semid_ds`结构：

```c
struct semid_ds{
    struct ipc_perm sem_perm;
    unsigned short sem_nsems; /* # of semaphores in set */
    time_t sem_otime;         /* last-semop() time */
    time_t sem_ctime;         /* last-change time */
};
```

每个信号量由一个无名结构表示，包含成员

```c
struct{
    unsigned short semval;     /* semaphore value */
    pid_t          sempid;     /* pid for last operation */
    unsigned short semncnt;    /* processes awaiting semval>curval */
    unsigned short semzcnt;    /* processes awaiting semval == 0 */
};
```

使用XSI信号量需先调用semget获得一个信号量ID。

```c
#include <sys/sem.h>
int semget(key_t key, int nsems, int flag);
//成功返回信号量ID；出错返回-1
```

创建新集合时，otime设置为0，ctime为当前时间，nsems为nsems

nsems是该集合中的信号量数。创建新集合（服务器进程），必须指定nsems。引用现有集合（客户进程），将nsems指定为0。

```c
#include <sys/sem.h>
int semctl(int semid, int semnum, int cmd, ... /* union semun arg */);
```

对信号量的操作，第四个参数可选，取决于所请求的命令。

```c
union semun{
    int val;                 /* for SETVAL */
    struct semid_ds *buf;    /* for IPC_STAT and IPC_SET */
    unsigned short *array;   /* for GETALL and SETALL */
};
```

通常是联合而非指向联合的指针。

cmd参数指定命令，运行在semid指定的信号量集合上的。针对特定信号量值，semnum指定信号量集合中成员，值在 0 - nsems-1，包括0和nsems-1。

`IPC_STAT`取集合`semid_ds`存放在arg.buf中

`IPC_SET`按arg.buf，设置`sem_perm`的uid，gid和mode字段。此命令只能由两种进程执行：有效用户ID等于`sem_perm`结构中字段cuid或uid，另一种具有超级用户特权进程。

`IPC_RMID`从系统中删除该信号量集合。同消息队列

`GETVAL`获取semnum的semval。

`SETVAL`设置semval，根据arg.val

`GETPID`返回sempid

`GETNCNT`和`GETZCNT`返回semncnt和semzcnt

`GETALL`和`SETALL`针对集合中所有信号量值，取或者设置，利用arg.array

成功返回0出错返回-1并设置errno。

```c
#include <sys/sem.h>
int semop(int semid, struct sembuf semoparray[], size_t nops);
//成功返回0 出错返回-1
```

自动执行信号量集合上的操作数组，semoparray指向由sembuf结构表示的信号量操作数组：

```c
struct sembuf{
    unsigned short sem_num; /* member int set (0,1...,nsems-1) */
    short          sem_op;  /*operation(negative,0,or positive) */
    short          sem_flg; /* IPC_NOWAIT, SEM_UNDO */
};
```

nops规定了数组操作数。

集合中操作由op规定：

1、op为正，进程释放占用资源数，op值加到信号量值，指定undo则**也**从该进程的此信号量调整值减去op。

2、op为负，表示要获取由该信号量控制的资源。

若该信号量值大于等于op绝对值，信号量值减去op绝对值，undo则绝对值也加到该进程此信号量调整值上。

若信号量值小于op绝对值，则：

a、指`IPC_NOWAIT`，semop出错返回EAGAIN。

b、未指定`IPC_NOWAIT`，semncnt值加1，进程被挂起直到：

    i、信号量值大于等于op绝对值，semncnt-1，且信号量值减去op绝对值

    ii、系统中删除了此信号量，返回EIDRM

    iii、进程捕捉到信号，从信号处理程序返回，semncnt-1，返回EINTR。

3、op为0，表示调用进程希望等待到该信号量值变为0。

信号量当前是0，立即返回

非0，则：

a、指`IPC_NOWAIT`，semop出错返回EAGAIN。

b、未指定`IPC_NOWAIT`，semzcnt值加1，进程被挂起直到：

    i、信号量值等于0，semnznt-1

    ii、系统中删除了此信号量，返回EIDRM

    iii、进程捕捉到信号，从信号处理程序返回，semzcnt-1，返回EINTR。

semmop具有原子性，或者执行数组中所有操作，或者一个不做。

exit时信号量的调整：信号量操作指定`SEM_UNDO`，再分配资源，内核会记住对该特定信号量，分配给调用进程的资源数，进程终止，内核检验进程是否仍有尚未处理的信号量调整值，有则按调整值对相应信号量处理。

用带SETVAL货SETALL命令的semctl设置一个信号量的值，所有进程中，信号量调整值都被设置为0。

多进程共享资源，信号量和记录锁以及互斥量。

## 共享存储

共享存储允许两个或多个进程共享一个给定的存储区。是最快的IPC，不需要多进程间复制数据。需同步访问一个给定的存储区。通常信号量用于同步共享存储访问。

共享存储的一种形式：多进程将同一文件映射到它们的地址空间。XSI共享存储和内存映射的文件不同之处在于，前者没有相关文件。XSI共享存储段是**内存的匿名段**。

内核为每个共享存储段维护着一个结构：

```c
struct shmid_ds{
    struct ipc_perm shm_perm;
    size_t          shm_segsz; /* size of segment in bytes */
    pid_t           shm_lpid;  /* pid of lash shmop() */
    pid_t           shm_cpid;  /* pid of creator */
    shmatt_t        shm_nattch;/* number of current attaches */
    time_t          shm_atime; /* last-attach time */
    time_t          shm_dtime; /* last-detach time */
    time_t          shm_ctime; /* last-change time */
    ...
};
```

`shmatt_t`为无符号整数，至少和unsigned short一样大。

```c
#include <sys/shm.h>
int shmget(key_t key, size_t size, int flag);
//成功返回共享存储ID；出错返回-1
```

获得共享存储标识符。

创建新段时，perm初始化，mode按flag中权限位设置，lpid、nattach、atime、dtime设置为0，ctime为当前时间，segsz为请求的size。

参数size是该共享存储段的长度，以字节为单位。实现通常将其向上取为系统页长的整倍数。若size并非整倍数，最后一页的余下部分不可使用。创建新段需指定size，引用现存段，size指定为0。创建新段内容初始化为0。

```c
#include <sys/shm.h>
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
//成功返回0；出错返回-1
```

cmd指定命令，在shmid指定的段上执行

`IPC_STAT`、`IPC_SET`、`IPC_RMID`

Linux和Solaris提供另外两种命令

`SHM_LOCK`和`SHM_UNLOCK`在内存中对共享存储段加锁和解锁，只能由超级用户执行。

```c
#include <sys/shm.h>
void *shmat(int shmid, const void *addr, int flag);
//成功返回指向共享存储段的指针，出错返回-1
```

共享存储段创建后，可用shmat将其连接到进程的地址空间。连接到调用进程的哪个地址上与addr参数及flag中是否指定`SHM_RND`位有关：

1、addr为0，此段连接到内核选择的第一个可用地址，推荐。

2、addr非0且没指定RND，则此段连接到addr指定地址。

3、addr非0，指定RND，此段连接到 (addr-(addr mod SHMLBA) 所表示地址。RND命令是取整，SHMLBA是低边界地址倍数。

除非只在一种硬件上运行应用程序，否则不应指定共享存储段所连接到的地址。应当指定addr为0。

flag中指定`SHM_RDONLY`位则只读方式连接此段，否则以读写方式连接。

shmat返回该段连接的实际地址。出错返回-1，成功执行则内核将**nattach计数器**加1。

对共享存储段操作结束，调用shmdt与该段分离，并不从系统删除其标识符及结构，直到某进程带`IPC_RMID`命令调用shmctl特地删除

```c
#include <sys/shm.h>
int shmdt(const void *addr);
//成功返回0；出错返回-1
```

addr参数是以前调用shmat时返回值，成功则**nattach计数器**减1。

使用/dev/zero设备调用mmap进行存储映射，无需存在实际文件。但只在两个相关进程间起作用，线程可能更简单有效，需对共享数据进行同步访问。

这是匿名存储映射，mmap需指定`MAP_ANON`，文件描述符指定为-1。

两个无关进程共享存储段，使用XSI共享存储函数，或用mmap将同一文件映射到它们的地址空间，使用`MAP_SHARED`标志。

## POSIX信号量

POSIX信号量接口解决XSI信号量接口的缺陷：

1、更高性能的实现

2、使用更简单：没有信号集，在熟悉的文件系统操作后一些接口被模式化

3、删除时表现更完美。XSI信号量被删除使用该信号量标识符操作会失败，errno设置为EIDRM，POSIX信号量，操作时正常工作直到该信号量的最后一次引用被释放。

存在两种形式：命名和未命名的。创建和销毁存在差异，未命名信号量只存在于内存，要求进程必须可访问内存。意味着只能应用在同一进程中的线程或者不同进程中映射相同内存内容到地址空间的中的线程。命名信号量可通过名字访问，可被任何已知名字的进程中线程用

```c
#include <semaphore.h>
sem_t *sem_open(const char *name, int oflag, ... /* mode_t mode,
                unsigned int value */);
//成功返回指向信号量的指针；出错返回SEM_FAILED
```

open用于创建新的命名信号量或使用现有信号量。

使用现有命名信号量时，仅指定 name和 oflag参数的0值。 oflag参数有`O_CREAT`标志集时，命名信号量不存在，则创建新的。已经存在则被使用，不会有额外初始化。

`O_CREAT`，需提供额外两参数，mode指定访问信号量的权限，可被调用者的文件创建屏蔽字修改。value指定信号量初始值，取值在`0~ SEM_VALUE_MAX`。

要确保创建的是信号量，设置oflag为`O_CREAT|O_EXCL`。已存在着open报错。

为增加可移植性，选择信号量命名时遵循的规则：

1、名字的第一个字符为斜杠（/）。

2、名字不应包含其他斜杠，避免实现定义的行为。

3、信号量名字的最大长度是实现定义的。

```c
#include <semaphore.h>
int sem_close(sem_t *sem);
//成功返回0 出错返回-1
```

完成信号量操作，调用close释放信号量相关资源

若未调用close退出进程，内核自动关闭任何打开的信号量，不影响信号量值状态。调用close，信号量值也不受影响。

```c
#include <semaphore.h>
int sem_unlink(const char *name);
//成功返回0 出错返回-1
```

删除信号量的名字。没有打开的信号量引用则信号量被销毁，否则延迟到最后一个打开的引用关闭。

```c
#include <semaphore.h>
int sem_trywait(sem_t *sem);
int sem_wait(sem_t *sem);
//成功返回0 出错返回-1
```

实现信号量的减1操作。

wait，信号量计数是0发生阻塞直到信号量减1，或被信号中断才返回。trywait避免阻塞，若为0则返回-1，errno为EAGAIN。

```c
#include <semaphore.h>
#include <time.h>
int sem_timewait(sem_t *restrict sem,
                    const struct timespec *restrict tsptr);
//成功返回0，出错返回-1
```

阻塞一段确定的时间。tsptr参数指定绝对时间，信号量可以立即减一，则超时值不重要，操作成功，否则超时到期，返回-1，errno为ETIMEOUT。

```c
#include <semphore.h>
int sem_post(sem_t *sem);
//成功返回0 出错返回-1
```

信号量值增加1，调用post时，若wait中阻塞，则进程被唤醒并被post增1的信号量计数会再次被wai减1。

单进程中使用未命名信号量更容易。

```c
#include <semaphore.h>
int sem_init(sem_t *sem, int pshared, unsigned int value);
//成功返回0；出错返回-1
```

创建未命名的信号量，pshared参数表明是否在多个进程中使用，是则非0。value指定信号量的初始值。

需声明`sem_t`类型的变量并把地址传递给init来实现初始化，两个进程间使用信号量，需确保sem参数指向两个进程间共享的内存范围。

```c
#include <semphore.h>
int sem_destroy(sem_t *sem);
//成功返回0 出错返回-1
```

对未命名信号量使用完成，destroy丢弃。调用后，不能再使用sem信号量函数，出发init重新初始化。

```c
#include <semaphore.h>
int sem_getvalue(sem_t *restrict sem, int *restrict valp);
//成功返回0， 出错返回-1
```

检测信号量值，valp包含信号量值，只能用于调试，读取的值可能改变。

## 客户进程-服务器进程属性

1、最简单关系类型，客户进程fork后exec所希望的服务器进程，fork前创建两个半双工管道。

2、服务器进程是守护进程，不适用管道，需使用命名IPC，如FIFO或消息队列。FIFO使用需服务器进程对每个客户进程有单独使用的FIFO。

使用消息队列存在多种可能：

a、服务器进程和所有客户进程间只使用一个队列，每个消息的类型字段指明谁是消息的接受者。

b、客户进程使用单独的消息队列。

使用消息队列的两种技术都可以用共享内存段和同步方法实现，

问题在于服务器进程准确标识客户进程。对于消息队列，客户进程和服务器进程使用专用队列，`msg_lspid`包含了对方进程的进程ID，但客户发送请求时，需客户进程的有效用户ID。

服务器进程标识客户进程，假设客户进程的有效用户ID是FIFO的所有者，利用stat或fstat获取，验证该FIFO只有用户读和用户写权限，还应检查三个时间量，与当前时间是否很接近。
