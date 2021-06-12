# 高级I/O

非阻塞I/O，记录锁，I/O多路转接（select和poll函数），异步I/O，readv和writev，存储映射I/O mmap

## 非阻塞I/O

“低速”系统调用：可能使进程永远阻塞的一类系统调用，包括：

1、某些文件类型（如读管道、终端设备和网络设备）的数据并不存在，读操作可能使调用者永远阻塞

2、如果数据不能被相同的文件类型立即接受（如管道中无空间、网络流控制），写操作可能使调用者永远阻塞。

3、在某种条件发送前打开某些文件类型可能会发生阻塞（如要打开一个终端设备，需要先等待与之连接的调制解调器应答；如若以只写模式打开FIFO，那么在没有其他进程已用读模式打开该FIFO时也要等待。）

4、对已经加上强制性记录锁的文件进行读写

5、某些ioctl操作

6、某些进程间通信函数

读写磁盘文件会暂时阻塞调用者，但并不能将与磁盘I/O有关的系统调用视为“低速”。

非阻塞I/O使我们可以发出open、read和write的I/O操作，并使这些操作不会永远阻塞。操作不能完成，则立即出错返回，表示该操作继续执行将阻塞。

对一个给定的描述符，有两种为其指定非阻塞I/O的方法：

1、如果调用open获得描述符，指定`O_NOBLOCK`标准

2、对已经打开的描述符，调用fcntl，由该函数打开`O_NOBLOCK`文件状态标志。

可将应用程序设计成使用多线程的，从而避免使用非阻塞I/O。若能在其他线程中继续执行，可允许单个线程在I/O调用中阻塞。线程间同步的开销可能增加复杂性，导致得不偿失的后果

## 记录锁

两人同时编辑一个文件时，大多数UNIX系统，该文件的最后状态取决于写该文件的最后一个进程。对于数据库等应用程序，进程有时需要确保它正在单独写一个文件。

记录锁的功能：当第一个进程正在读或者修改文件的某个部分时，使用记录锁可以阻止其他进程修改同一文件区。 又称**字节范围锁**，锁定的是文件中的一个区域（或整个文件）。

### 历史

早期UNIX系统不能用于运行数据库系统，原因在于UNIX系统不支持对部分文件加锁。

早期伯克利版本只支持flock函数，对整个文件加锁，而不能对文件中的一部分加锁。

SVR3通过fcntl函数增加了记录锁功能，在此基础上构造了lockf函数，提供了一个简化的接口。这些函数运行调用者对文件中任意字节数的区域加锁，一个字节至整个文件。

### fcntl记录锁

```c
#include <fcntl.h>
int fcntl(int fd, int cmd, .../*struct flock *flockptr */);
//成功则依赖于cmd，否则返回-1
```

对于记录锁，cmd为`F_GETLK F_SETLK或F_SETLKW`。第三个参数为指向flock结构的指针。

```c
struct flock {
    short l_type;    /* F_RDLCK, F_WRLCK, F_UNLCK */
    short l_whence;  /* SEEK_SET, SEEK_CUR, SEEK_END */
    off_t l_start;   /* offset in bytes, relative to whence */
    off_t l_len;     /* length in bytes, 0 means lock to EOF */
    pid_t l_pid;     /* returned with F_GETLK */
}
```

ltype为锁类型，RDLCK 共享读锁； WRLCK 独占性写锁； 或 UNLCK 解锁一个区域

lstart和lwhence为要加锁或解锁区域的起始字节偏移量。

llen为区域的字节长度

lpid 进程ID持有的锁能阻塞当前进程，仅由`F_GETLK` 返回

加锁和解锁区域的说明需注意以下规则：

1、指定区域起始偏移量的两个元素与lseek函数类似

2、锁可以在当前文件尾端处开始或者越过尾端处开始，但不能在文件起始位置之前开始。

3、llen为0，**表示锁的范围可以扩展到最大可能偏移量**，即不管向文件追加写多少数据，都在锁的范围，且起始位置可以是文件中任意一个位置。

4、对整个文件加锁，设置lstart和lwhence指向文件起始位置，指定llen为0。

共享读锁和独占性写锁：任意多个进程在一个给定字节上可以有一把共享的读锁，但只能有一个进程有一把独占写锁。如果一个给定字节已有读锁，不能加写锁，如果有写锁，则不能加任何读锁。

如果一个进程对一个文件区已有一把锁，若又企图在同一文件区再加一把锁，则新锁将替换已有锁。

加读锁，该描述符必须是读打开，加写锁，该描述符必须是写打开。

1、`F_GETLK` 判断flockptr所描述的锁是否会被另一把锁排斥（阻塞）。若存在一把锁阻止创建有flockptr所描述的锁，该现有锁的信息将重写flockptr指向的信息。若不存在，则ltype设置为`F_UNLCK`，flockptr所指向结构中的其他信息保持不变。

2、`F_SETLK` 设置由flockptr所描述的锁。如果试图获取一把读锁或写锁，而兼容性规则阻止获取，fcntl出错返回，errno设置为EACCES或EAGAIN。 也可用于清楚flockptr指定的锁。

3、`F_SETLKW` 是`F_SETLK`的阻塞版本，W为wait，如果所请求的读锁或写锁因另一个进程对请求区域某部分已经加锁而不能授予，调用进程被置为休眠。若请求创建的锁已可用，或休眠由信号中断，则该进程被唤醒。

用GETLK测试能否建立一把锁，然后SETLK或SETLKW企图建立这把锁，这两者不是一个原子操作。不能保证fcntl两次调用间不会有其他进程插入并建立相同锁。若不希望等待锁变为可用时产生阻塞，必须处理SETLK返回的可能的出错。

POSIX并没有说明：在进程一个区间加读锁，第二个进程试图加写锁时阻塞，第三个进程试图加读锁是否被允许。允许则可能造成写锁进程被饿死。

**在设置或释放文件上的一把锁时，系统按要求组合或分裂相邻区**。如100-199字节被加锁，150字节被解锁，则100-149以及151-199字节存在两把锁，内核需维持两把锁。若再对150字节加锁，则3个相邻加锁区被合并成一个区。

### 锁的隐含继承和释放

记录锁的自动继承和释放遵循规则：

1、锁与进程和文件两者相关联。当一个进程终止，它建立的锁全部释放；无论一个描述符何时关闭，该进程通过**这一描述符引用的文件**上的任何一把锁都会释放（这些锁都是该进程设置的）。

即使是dup或者open产生的fd2，若fd1与fd2引用文件相同，fd1锁在fd2被关闭时被释放。

2、由fork产生的子进程不继承父进程所设置的锁。进程得到锁，再fork，子进程被视为另一个进程。对于通过fork从父进程处继承过来的描述符，子进程需调用fcntl才能获得它自己的锁。目的在于防止父子进程同时写同一个文件。

3、在执行exec后，新程序可以继承原执行程序的锁。若设置执行时关闭标志，则exec关闭该fd时，将释放相应文件的所有锁。

### FreeBSD实现

![](C:\Users\asuss\AppData\Roaming\marktext\images\2021-06-07-21-30-05-image.png)

父进程中，fd2为dup(fd1)， fd3和fd1为open同一路径文件。记录锁在原来结构上新加了lockf结构，由i节点结构开始相互链接起来。关闭fd1、fd2、fd3任意一个都将释放父进程创建的写锁。关闭时，内核会从该fd所关联的i节点开始，逐个检查lockf链接表中的各项，释放由调用进程持有的各把锁。

### 在文件尾端加锁

对相对于文件尾端的字节范围加锁或解锁时，常常需要相对于文件当前长度指定一把锁，但fstat和锁调用之间，可能会有另一进程更改文件长度。

对文件的一部分加锁时，内核将指定的偏移量变换成绝对文件偏移量。即`SEEK_SET`，除了绝对偏移量，fcntl还允许相对于文件中某个点指定偏移量，`SEEK_CUR`或`SEEK_END`。CUR和END可能不断变化，但这种变化不应影响现有锁的状态，所以内核必须独立于当前文件偏移量或文件尾端而记住锁。

### 建议性锁和强制性锁

数据库访问例程库。该库中所有函数以一致的方法处理记录锁，则称使用这些函数访问数据库的进程集为**合作进程**。如果这些函数是唯一地用来访问数据库的函数，则它们使用建议性锁是可行的。

**建议性锁**不能阻止对数据库文件有写权限的任何其他进程写这个数据库文件。不使用数据库访问例程库协同一致的方法来访问数据库的进程是非合作进程。

**强制性锁**会让内核检查每个open、read和write，验证调用进程是否违背了正在访问的文件上的某一把锁。又称强迫方式锁。Linux中在各个文件系统基础上 mount的-o mand选项

对一个特定文件打开其设置组ID位、关闭其组执行位便开启了对该文件的强制性锁机制。组执行位关闭时，设置组ID位不再有意义。两者组合可指定对该文件的锁是强制性的。

若进程试图读写一个强制性锁起作用的文件，而读写部分已被上锁。结果由操作类型，其他进程持有锁类型，read或write描述符是阻塞还是非阻塞决定。

如果欲打开的文件具有强制性记录锁，而open调用标志为`O_TRUNC`或`O_CREAT`，open立即出错返回，errno设置为EAGAIN。

Solaris对creat出错，linux允许creat标志，文件不存在时才创建，而有锁文件必存在。

open处理在UNIX环境中，若打开一个具有强制性锁的文件，并为强制性读锁，休眠期间对文件处理：

1、ed编辑器对该文件编辑操作，结果可写回磁盘。强制性锁失效。ed将新内容写到临时文件，删除原文件，再将临时文件名改为原文件名。

2、vi编辑器不可编辑该文件，可读内容。

3、Korn shell和Bourne shell的 > 和 >> 操作符重写或追加写该文件，K出错，B的追加在creat时不报错。

强制性锁是可以设法避开的。

## I/O多路转接 多路复用

从一个描述符读，然后写到另一个描述符时，可循环使用**阻塞I/O**， while(read) write。

telnet进程有两个输入两个输出，不能对两个输入中任一个使用阻塞read

可**通过fork**，将telnet命令调用进程，变成两个进程，每个进程处理一条数据。存在操作终止问题，两个进程间需要通过信号通知，一个进程的两个线程可以避免终止复杂性，但需要两个线程同步。

另一种方法使用一个进程执行该程序，使用**非阻塞I/O**读取数据。基本思想：两个输入描述符都设置为非阻塞的，对第一个描述符发一个read。若该输入有数据，则读数据并处理。无数据则调用立即返回。第二个描述符同样处理。此后，等待一定的时间，然后再尝试从第一个描述符读。这种形式的循环称为**轮询**。

缺点：浪费CPU时间，大多数时间无数据，read系统调用浪费时间，循环后等多长时间再执行下一轮循环也很难确定。多任务系统应当避免使用这种方法。

**异步I/O**。利用这种技术，进程告诉内核：当fd准备好可以进行I/O时，用一个信号通知它。存在两个问题，可移植性存在问题；这种信号对每个进程而言只有一个（SIGPOLL或SIGIO）。如果该信号对两个fd都起作用，进程无法判别哪个fd准备好了。

**I/O 多路转接**：先构造一张我们感兴趣的fd表，调用一个函数，直到这些描述符中的一个已经准备好进行I/O时，该函数才返回。从这些函数返回时，进程会被告知哪些描述符已准备好可以进行I/O。

### 函数select和pselect

```c
#include <sys/select.h>
int select(int maxfdp1, fd_set *restrict readfds,
            fd_set *restrict writefds, fd_set *restrict exceptfds,
            struct timeval *restrict tvptr);
// 返回准备就绪的描述符数目；超时返回0；出错返回-1
```

select参数告诉内核：

1、我们所关心的描述符；

2、对于每个描述符我们所关心的条件（是否想从一个给定的描述符读，是否想写给一个指定的描述符，是否关心一个给定描述符的异常条件）；

3、愿意等待多长时间（可永远等待、等待一个固定的时间或者根本不等待）；

4、已准备好的描述符的总数量；

5、对于读、写或异常这三个条件中的每一个，哪些描述符已准备好。

tvptr为NULL，为永远等待，如果捕捉到一个信号则中断此等待。当所指定的描述符中的一个已准备好或捕捉到一个信号则返回。捕捉到信号返回-1，errno设置为EINTR；

`tvptr->tv_sec == 0 && tvptr->tv_usec == 0`根本不等待。测试所有指定的描述符并立即返回。这是轮询系统找到多个描述符状态而不阻塞select函数的方法。

`tvptr->tv_sec != 0 || tvptr->tv_usec != 0`等待指定的秒数和微秒数。指定的描述符之一已准备好，或指定的时间值已经超过时立即返回。超时到期无准备好的描述符返回0，等待可被捕捉到的信号中断。

readfds、writefds、exceptfds是指向描述符集的指针。说明了可读、可写或处于异常条件的描述符集合。每个描述符集存储在一个`fd_set`数据类型，可认为是一个很大的字节数组

唯一可进行的处理：分配一个这种类型的变量，将这种类型一个变量值赋给同类型的另一个变量，或对这种类型变量使用下列4个函数之一：

```c
#include <sys/select.h>
int FD_ISSET(int fd, fd_set *fdset);
//返回值，若fd在描述符集中，返回非0，否则返回0
void FD_CLR(int fd, fd_set *fdset);
void FD_SET(int fd, fd_set *fdset);
void FD_ZERO(fd_set *fdset);
```

这些接口可实现为宏或函数。ZERO将set变量所有位设置为0，开启描述符集中的一位，可调用SET函数。CLR可以清除一位。ISSET可以测试描述符集中指定位是否已打开。

声明描述符集后，必须使用ZERO将描述符集置0，然后设置各个描述符的位。

select返回时，可以ISSET测试该集中的给定位是否仍处于打开状态。

select中间的3个描述符集指针任意一个（或全部）可以是空指针，表示对相应条件并不关心。若均为NULL，select提供比sleep更精确的定时器。

select第一个参数maxfdp1，为最大fd编号值+1，在三个描述符集中找到maxfd，然后加1。可设置为`FD_SETSIZE`，为常量，指定最大描述符（经常是1024）。大多数程序只使用3-10个fd。

因为描述符编号从0开始，所以最大描述符编号值+1，实际上为检查的描述符数，从fd0开始

select的三个返回值：

1、返回-1表示出错，所指定的fd一个都没准备好，捕捉到信号，此情况一个描述符集不修改

2、返回0表示没有fd准备好，超时，所有描述符集会置0

3、一个正返回值说明了已准备好的描述符数，为3个描述符集中的已准备好的描述符数之和，这种情况下，3个描述符集仍旧打开的位对应于已准备好的描述符。

对“准备好”含义说明：

1、对readfds中的一个fd进行read操作不会阻塞，则该描述符准备好

2、对writefds中的一个fd进行write操作不会阻塞，则该描述符准备好

3、对exceptfds中的一个fd有一个未决异常条件，认为此描述符是准备好的。包括：在网络连接上到达**带外**的数据，或者处于数据包模式的伪终端上发生某些条件

4、对于读、写和异常条件，普通文件的fd总是返回准备好。

一个描述符阻塞并不意味着select阻塞，可设置select等待时间。

一个描述符碰到文件尾端，select会认为**该fd是可读的**，调用read返回0。

```c
#include <sys/select.h>
int pselect(int maxfdp1, fd_set *restrict readfds,
            fd_set *restrict writefds, fd_set *restrict exceptfds,
            const struct timespec *restrict tsptr,
            const sigset_t *restrict sigmask);
//返回准备就绪的描述符数目；超时返回0 出错返回-1
```

POSIX.1定义的select的变体，其与select的不同在于：

1、select超时使用timeval结构指定，pselect使用timespec，以秒和纳秒而非秒和微秒，平台支持则更精确。

2、超时值为const，保证pselect不会改变此值

3、pselect可使用可选信号屏蔽字。sigmask为NULL，则信号有关方面pselect与select一致。否则，调用pselect，以原子操作的方式安装该信号屏蔽字。返回时，恢复以前的信号屏蔽字。

### 函数poll

```c
#include <poll.h>
int poll(struct pollfd fdarray[], nfds_t nfds, int timeout);
//返回准备就绪的描述符数目；超时返回0 出错返回-1
```

poll类似于select，poll可用于任何类型的文件描述符。

与select不同，poll不为每个条件构造描述符集，而是构造一个pollfd数组，每个数组元素指定一个fd和条件

```c
struct pollfd{
    int fd;         /*file descriptor to check, or < 0 to ignore */
    short events;   /* events of interest on fd */
    short revents;  /* events that occurred on fd */
}
```

fdarray数组元素数由nfds指定。events成员设置用于告诉内核我们关心的是每个描述符的哪些事件。返回时，revents成员由内核设置用于说明每个描述符发生了哪些事件。（poll不更改events，select修改参数来表示已好的fd）。

events包括：可读性的标志 POLLIN POLLRDNORM POLLRDBAND POLLPRI；可写性的标志 POLLOUT POLLWRNORM POLLWRBAND；异常条件标志：POLLERR POLLHUP POLLNVAL。

异常条件是内核在返回时设置的，即使events未设置，revents也会返回。

当一个fd被挂断POLLHUP后，不能写该描述符，但可能仍然可以从该描述符读取到数据。

poll的最后一个参数指定愿意等待时间：

1、timeout == -1，永远等待。当所指定的描述符中的一个准备好，或捕捉到一个信号时返回。捕捉到信号，返回-1，errno设置为EINTR

2、timeout == 0， 不等待。测试所有描述符并立即返回。

3、timeout > 0，等待timeout毫秒。指定的fd之一已准备好，或timeout到期时立即返回。到期还没有fd准备好，返回0。

文件尾端与挂断之间的区别：正从终端读入数据，并键入文件结束符，则会打开POLLIN，并read返回0，revents中POLLHUP未打开。若正在读调制解调器，且电话线已挂断，则收到POLLHUP通知。

**select和poll的可中断性**：中断系统调用的自动重启功能，select和poll通常不重启动，大多数系统指定`SA_RESTART`也是这样，SVR4指定了则自动重启。为了阻止重启动，若信号可中断select或poll，需要使用`signal_intr`函数。

## 异步I/O

select和poll可以实现异步形式的通知，关于fd状态，系统并不主动告诉我们任何信息，我们需要进行查询。BSD和Syetem V派生的所有系统都提供了异步I/O。

使用一个信号（System V中SIGPOLL，BSD中SIGIO）通知进程，对某个描述符所关心的某个事件已经发生。该形式异步I/O受限制：不能用于所有文件类型，且只能使用一个信号。对一个以上的描述符进行异步I/O，进程接收到该信号时不知道信号对应的描述符。

异步I/O的成本，通过选择来灵活处理多个并发操作。简单的做法可使用多线程，使用同步模型编写程序，并让多个线程以异步方式运行。

POSIX异步I/O接口的缺点：

1、每个异步操作有3处可能产生错误的地方：操作提交的部分；操作本身的结果；用于决定异步操作状态的函数中。

2、与POSIX异步I/O接口的传统方法比，它们本身涉及大量的额外设置和处理规则。

3、从错误中恢复可能比较困难。如提交多个异步写操作，若操作相关，其中一个失败则所有成功的写操作需被撤销。

### System V异步I/O

异步I/O是STREAMS系统的一部分，只对STREAMS设备和STREAMS管道起作用，信号是SIGPOLL。

对一个STREAMS设备启动异步I/O，需要调用ioctl，其第二个参数request设置为`I_SETSIG`，第三个参数则是一个或多个常量构成的整型值。

常量包括读操作`S_INPUT S_RDNORM S_RDBAND S_BANDURG S_HIPRI` 写操作`S_OUTPUT S_WRNORM S_WRBAND` 异常条件`S_MSG S_ERROR S_HANGUP`

除了调用ioctl指定产生SIGPOLL信号的条件外，还应当为该信号建立信号处理程序。SIGPOLL默认动作是终止进程，应当在**调用ioctl之前**建立信号处理程序。

### BSD异步I/O

异步I/O是信号SIGIO和SIGURG的组合。SIGIO是通用异步I/O信号，SIGURG则只用来通知进程网络连接上的带外数据已经到达。

为接收SIGIO信号，需执行以下3步：

1、调用signal或sigaction为SIGIO信号建立信号处理程序。

2、以命令`F_SETOWN`调用fcntl设置进程ID或进程组ID，用于接收对该描述符的信号。

3、以命令`F_SETFL`调用fcntl设置`O_ASYNC`文件状态标志，使在该描述符上可以进行异步I/O。

第3步仅能对指向**终端或网络**的描述符执行。

对于SIGURG信号，只需执行第1步和第2步。该信号仅对引用支持带外数据的网络连接描述符而产生，如TCP连接（URG位）。

### POSIX异步I/O

POSIX异步I/O接口为对不同类型的文件进行异步I/O提供了一套一致的方法。

异步I/O接口使用AIO控制块来描述I/O操作。aiocb结构定义了AIO控制块。

```c
struct aiocb{
    int             aio_fildes;    /*file descriptor */
    off_t           aio_offset;    /*file offset for I/O */
    volatile void  *aio_buf;       /*buffer for I/O */
    size_t          aio_nbytes;    /*number of bytes to transfer */
    int             aio_reqprio;   /*priority */
    struct sigevent aio_sigevent;  /*signal information */
    int             aio_lio_opcode;/*operation for list I/O */
};
```

fildes字段表示被打开用于读或写的fd，读或写操作从offset指定的偏移量开始，读操作，数据复制到缓冲区，buf指定的地址开始。写操作，数据从缓冲区复制出来。nbytes字段包括要读或写的字节数。

**异步I/O必须显式地指定偏移量**。接口并不影响由OS维护的文件偏移量。只要不在同一个进程里把异步I/O函数和传统I/O函数混用在同一文件，就不会导致问题。若异步I/O接口向一个以追加模式（`O_APPEND`）打开的文件中写入数据，AIO控制块中的offset被忽略。

reqprio字段为异步I/O请求提示顺序。系统不一定遵循该提示。aio_lio_opcode字段只能用于基于列表的异步I/O，sigevent字段控制在I/O事件完成后，如何通知应用程序。

```c
struct sigevent{
    int            sigev_notify;        /* notify type */
    int            sigev_signo;         /* signal number */
    union sigval   sigev_value;         /* notify argument */
    void (*sigev_notify_function)(union sigval); /*notify function*/
    pthread_attr_t *sigev_notify_attributes; /*notify attrs*/
};
```

notify字段控制通知的类型，取值可为：

`SIGEV_NONE` 异步I/O请求完成后，不通知进程。

`SIGEV_SIGNAL` 异步I/O请求完成后，产生由sigev_signo字段指定的信号。若应用程序已选择捕捉该信号，且在建立信号处理程序的时候指定了`SA_SIGINFO`标志，则该信号被入队。信号处理程序会传送给一个siginfo结构，该结构的`si_value`字段被设置为`sigev_value`（若使用`SA_SIGINFO`标志）。

`SIGEV_THREAD` 异步I/O请求完成，由`sigev_notify_function`字段指定的函数被调用。`sigev_value`字段作为唯一参数。除非`sigev_notify_attributes`字段被设定为pthread属性结构的地址，且该结构指定了一个另外的线程属性，否则该函数将在分离状态下的一个单独线程中执行。

进行异步I/O前需初始化AIO控制块，而后异步读写操作

```c
#include <aio.h>
int aio_read(struct aiocb *aiocb);
int aio_write(struct aiocb *aiocb);
//成功返回0， 出错返回-1
```

当这些函数返回成功时，异步I/O请求便被OS放入等待处理的队列中了，返回值和实际I/O没有关系。I/O操作在等待时，必须确保AIO控制块和数据库缓冲区保持稳定；它们下面对应的**内存**必须始终是合法的，除非I/O操作完成，否则**不能被复用**。

```c
#include <aio.h>
int aio_fsync(int op, struct aiocb *aiocb);
//成功返回0 出错返回-1
```

强制所有等待中的异步操作不等待而写入持久化的存储中。

fildes字段指定了其异步写操作被同步的文件。若op参数为`O_DSYNC`，操作执行就像fdatasync，否则op为`O_SYNC`，操作执行像fsync。

在安排了同步时，fsync操作返回。在异步同步操作完成之前，数据不会被持久化。AIO控制块控制如何被通知。

```c
#include <aio.h>
int aio_error(const struct aiocb *aiocb);
//返回4种情况
```

为获知一个异步读、写或者同步操作的完成状态。返回4种值：

1、0， 异步操作成功完成。需调用aio_return 函数获取操作返回值。

2、-1，对error调用失败，errno会说明错误原因。

3、EINPROGRESS，异步读写或同步操作仍在等待。

4、其他情况，其他任何返回值是相关的异步操作失败返回的错误码。

异步操作成功，需调用return获取异步操作的返回值。

```c
#include <aio.h>
ssize_t aio_return(const struct aiocb *aiocb);
```

直到异步操作完成之前，都需要小心不要调用异步返回函数。操作完成前的结果是未定义的。对每个异步操作只调用一次aio_return。一旦调用了该函数，OS可以释放掉包含I/O操作返回值的记录。

若调用异步返回失败，返回-1，并设置errno。其他情况下将返回异步操作的结果，即*read*，*write*或者*fsync*在被成功调用时可能返回的结果。

执行I/O时，若还有其他事务要处理而不想被I/O操作阻塞，可以使用异步I/O，若完成所有事务时仍有异步操作未完成，可使用aio_suspend函数阻塞进程，直到操作完成。

```c
#include <aio.h>
int aio_suspend(const struct aiocb *const list [], int nent,
                const struct timespec *timeout);
//成功返回0，出错返回-1
```

suspend可能返回三种情况。

若被信号中断，返回-1，errno为EINTR。

没有I/O操作完成，阻塞时间超过timeout参数指定的时间限制，返回-1，errno为EAGAIN，不设时间限制则timeout为空指针。

有任何I/O操作完成，返回0，若所有异步I/O都已经完成，那么不阻塞直接返回。

list指向AIO控制块数组，nent表明数组中条目数。数组中空指针被跳过，其他条目指向已用于初始化异步I/O操作的AIO控制块。

```c
#include <aio.h>
int aio_cancel(int fd, struct aiocb *aiocb);
```

不想再完成的等待中的异步I/O操作，可**尝试**cancel函数来取消。

fd参数指定了未完成的异步I/O操作的fd，aiocb为NULL，系统将尝试取消该文件上所有未完成的异步I/O操作。其他情况下，系统将尝试取消由AIO控制块描述的单个异步I/O操作。

cancel可能返回4个情况：

`AIO_ALLDONE`，所有操作在尝试取消它们之前已经完成。

`AIO_CANCELED`，所有要求的操作已被取消。

`AIO_NOTCANCELED`至少有一个要求的操作没有被取消

-1，对cancel调用失败，errno保存了错误码。

若异步I/O被成功取消，相应的aiocb调用error会返回错误ECANCELED。操作不能被取消，则aiocb不会被修改。

```c
#include <aio.h>
int lio_listio(int mode, struct aiocb *restrict const list[restrict],
                int nent, struct sigevent *restrict sigev);
//成功返回0，出错返回-1
```

既能同步使用，又能异步使用，提交一系列由一个AIO控制块列表描述的I/O请求。

mode参数决定了I/O是否真的是异步的。`LIO_WAIT`，函数在所有由列表指定的I/O操作完成后返回，sigev参数将被忽略。`LIO_NOWAIT`，将在I/O请求入队后立即返回。在所有I/O操作完成后，进程按照sigev指定地被异步通知。sigev为NULL则不被通知。

每个AIO控制块本身可能启用了各自操作完成时的异步通知，sigev参数指定的异步通知是在此之外**另加的**，且只会在所有I/O操作完成后发送。

list指向AIO控制块列表，指定了要运行的I/O操作。nent参数指定了数组中的元素个数，AIO控制列表中的NULL将被忽略。

每个aio控制块，aio_lio_opcode字段指定了该操作是一个读操作`LIO_READ`，写操作`LIO_WRITE`，还是将被忽略的空操作`LIO_NOP`。读操作会按照对应的AIO控制块传给aio_read函数处理。

实现会限制我们不想完成的异步I/O操作的数量。为运行时不变量，通过sysconf设置name参数来更改限制值

`AIO_LISTIO_MAX`单个列表I/O调用中最大I/O操作数 最小值2

`AIO_MAX`未完成的异步I/O操作的最大数目 最小值1

`AIO_PRIO_DELTA_MAX`进程可以减少的其异步I/O优先级的最大值 最小值0

## 函数readv和writev

```c
#include <sys/uio.h>
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
//成功返回已读或已写的字节数；出错返回-1
```

用于在一次函数调用中读写多个**非连续缓冲区**，可称为散布读和聚集写。

```c
struct iovec{
    void *iov_base; /* starting address of buffer */
    size_t iov_len; /* size of buffer */
};
```

iov数组元素数由iovcnt指定，最大值受限于`IOV_MAX`。

writev函数从缓冲区中聚集输出数据的顺序：iov[0] 到 iov[iovcnt - 1] 。writev返回输出的字节数，通常应等于所有缓冲区长度之和。

readv函数则将读入的数据按上述同样顺序散布到缓冲区中。readv总是先填满一个缓冲区，再填写下一个。readv返回读到的字节总数。遇到文件尾端，无数据可读，返回0。

系统调用次数尽量少，少量数据通过复制到一个缓冲区再write时间上优于writev。

## 函数readn和writen

管道、FIFO以及某些设备（特别是终端和网络）具有两种性质：

1、一次read操作所返回的数据可能少于所要求的数据，即使还没达到文件尾端也可能是这样。这不是一个错误，应当继续该设备。

2、一次write操作的返回值也可能少于指定输出的字节数。可能由某个因素导致，如内核输出缓冲区变满。非错误应当继续写余下的数据。（通常只有非阻塞描述符或捕捉到一个信号时，write中途返回。）

对于磁盘文件的读写并没有上述情况，除非文件系统用完空间，或接近配额限制，不能将要求写的数据全部写出。

```c
#include "apue.h"
ssize_t readn(int fd, void *buf, size_t nbytes);
ssize_t writen(int fd, void *buf, size_t nbytes);
//返回读写的字节数；出错返回-1
```

读写指定的N字节数据，并处理返回值可能小于要求值的情况。这两函数只是按需多次调用read、write直至读写了N字节数据。

仅当事先知道要接收数据的数量时，才调用readn。

在已经读、写了一些数据后出错，则函数返回已传输的数据量而非错误。读时到达文件尾端，返回已复制到调用者缓冲区中的字节数。

## 存储映射I/O

将一个磁盘文件映射到存储空间中的一个缓冲区上，当从缓冲区中取数据相当于读文件中的相应字节。数据存入缓冲区时，相应字节自动写入文件。可在不使用read和write情况下执行I/O。

```c
#include <sys/mmap.h>
void *mmap(void *addr, size_t len, int prot, int flag, int fd, off_t off);
//成功返回映射区的起始地址；出错返回MAP_FAILED
```

内核将一个给定的文件映射到一个存储区域中。

addr参数用于指定映射存储区的起始地址。通常设置为0，表示由系统选择该映射区的起始地址。函数返回值是该映射区起始地址。

fd为被映射文件的描述符。文件映射到地址空间前，必须先打开该文件。len参数为映射的字节数，off是要映射字节在文件中的起始偏移量。

prot参数指定了映射存储区的保护要求。

`PROT_READ`映射区可读；`PROT_WRITE`映射区可写；`PROT_EXEC`映射区可执行；`PROT_NONE`映射区不可访问；

可设置为NONE也可设置为rwx的任意组合按位或。对指定映射存储区的保护要求不能超过文件open模式访问权限。若该文件是只读打开，则映射存储区不能指定WRITE。

![](C:\Users\asuss\AppData\Roaming\marktext\images\2021-06-11-22-21-19-image.png)

flag参数影响映射存储区的多种属性：

`MAP_FIXED`返回值必须等于addr，不利于可移植性，若未指定此标志，addr非0，内核只把addr视为在何处设置映射区的一种建议，不保证会使用所要求的地址。addr指定为0可移植性最大。

`MAP_SHARED`描述了本进程对映射区所进行的存储操作的配置。指定存储操作修改映射文件，相当于该文件的write。必须指定该标志或PRIVATE，不能同时指定两者。

`MAP_PRIVATE`对映射区的存储操作导致创建该映射文件的一个私有副本。所有后来对该映射区的引用都是引用该副本。（用于调试程序，将程序文件的正文部分映射至存储区，允许用户修改指令，只影响程序文件的副本不影响原文件。）

off的值和addr的值通常被要求是系统虚拟存储页长度的倍数，off和addr常常指定为0，要求一般不重要。

与映射区相关的信号有SIGSEGV和SIGBUS。信号SIGSEGV通常用于指示进程试图访问对它不可用的存储区。如存入mmap设置的只读存储区。如果映射区的某个部分在访问时已不存在，则产生SIGBUS信号。如进程访问文件被截去部分的映射区。

子进程能通过fork**继承存储映射区**（复制父进程地址空间，包含了存储映射区），exec不能继承存储映射区。

```c
#include <sys/mman.h>
int mprotect(void *addr, size_t len, int prot);
//成功返回0 出错返回-1
```

更改一个现有映射的权限。

prot合法值与mmap中prot参数一样。addr值必须是系统页长的整数倍。

若修改的页是`MAP_SHARED`标志映射到地址空间，修改不会立即写回到文件。由内核守护进程决定何时写回脏页，依据系统负载和用于限制在系统失败事件中数据损失的配置参数。

共享映射的页已修改，可调用msync将页被冲洗到被映射的文件。类似fsync，作用于存储映射区。

```c
#include <sys/mman.h>
int msync(void *addr, size_t len, int flags);
//成功返回0 出错返回-1
```

映射是私有的，则不修改被映射的文件。地址必须与页边界对齐。

flags参数控制某种程度的冲洗存储区。`MS_ASYNC`标志简单地调试要写的页。若希望在返回前等待写操作完成，可使用`MS_SYNC`，两者必须指定一个。

`MS_INVALIDATE`为可选标志，允许通知OS丢弃与底层存储器没有同步的页。

进程终止时，会自动解除存储映射区的映射，munmap函数也可解除映射区。关闭映射存储区时使用的fd并不解除映射区。

```c
#include <sys/mman.h>
int munmap(void *addr, size_t len);
//成功返回0 出错返回-1
```

munmap并不影响被映射的对象。即映射区内容不会写到磁盘文件。对于SHARED区磁盘文件的更新，在数据写到存储映射区后的某个时刻，按内核虚拟存储算法自动进行。存储区解除映射后，PRIVATE存储区的修改会被丢弃。

read和write是内核缓冲区与应用缓冲区的复制，mmap和memcpy则是内核缓冲区复制到另一个内核缓冲区。

存储映射I/O处理的是存储空间而不是读、写一个文件，常常可以简化算法。适用于对帧缓冲设备的操作，该设备引用位图式显示。
