# 信号

信号是软件中断，提供了一种处理异步事件的方法。

## 信号概念

每个信号都有名字，以SIG开头。头文件`<signal.h>`中信号名被定义为正整数常量。不存在编号为0的信号，为空信号，kill(0)，有特殊响应。

产生信号的条件：

用户按某些终端键；硬件异常（除数为0、无效内存引用等）；进程调用kill(2) 将任意信号发送给其他进程或进程组，接收信号进程和发送信号进程的所有者必须相同，或者发送信号进程所有者为超级用户；用户用kill(1) 将信号发送给其他进程，为kill函数的接口，用于终止失控的后台进程；检测到某软件条件已经发生，应将其通知有关进程时产生信号。

信号是异步事件的经典实例。产生信号的事件对进程而言是随机出现，进程必须告诉内核信号发生时的下一步操作。

内核对信号的处理：

1、忽略信号，大多数信号都是，SIGKILL和SIGSTOP不能被忽略，它们向内核和超级用户提供了使进程终止或停止的可靠方法。

2、捕捉信号，通知内核在某信号发生时，调用用户函数，用户函数中执行用户希望对这事情进行的处理。不能捕捉SIGKILL和SIGSTOP信号。

3、执行系统默认动作。大多数系统默认动作为终止该进程。

终止+core 表示进程在当前工作目录的core文件复制了该进程的内存映像，系统调试程序使用core文件检查进程终止时的状态。

不产生core文件的条件：进程设置用户ID且当前用户非程序文件的所有者；进程是设置组ID的，当前用户非程序文件的组所有者；用户没有写当前工作目录的权限；文件已存在且用户对该文件没有写权限；文件太大

## 函数signal

```c
#include <signal.h>
void (*signal(int signo, void (*func)(int)))(int);
//成功返回以前信号处理配置，出错返回SIG_ERR
```

signo为信号名， func值为常量`SIG_IGN、SIG_DFL`或当接到次信号后要调用的函数地址，ign为忽略，dfl为系统默认动作。指定函数地址，信号发生调用该函数，捕捉信号，此函数为信号处理程序或信号捕捉函数。

signal函数要求两参数， 返回一个函数指针，该指针所指向的函数无返回值。signal返回值是函数地址，该函数有一个整型参数。

调用signal设置信号处理程序时，第二个参数指向该函数指针，返回值则是指向在此之前的信号处理程序的指针。

`typedef void Sigfunc(int)` Sigfunc为带(int)参量 无返回值的函数。

`#define SIG_ERR (void(*)( ))-1`

kill(1)和kill(2)只是将一个信号发送给一个进程或进程组。信号是否终止进程则取决于该信号的类型，以及进程是否安排了捕捉该信号。

### 程序启动

执行程序，信号状态都是系统默认或忽略。通常都被设置为默认动作，除非调用exec的进程忽略该信号。exec函数将原先设置为要捕捉的信号都更改为默认动作，其他信号状态不变。

（一个进程原先要捕捉的信号，当其执行一个新程序后，不能再捕捉了，信号捕捉函数的地址在新程序文件中失去意义。）

不改变信号的处理方式就不能确定信号的当前处理方式。

### 进程创建

进程调用fork，子进程继承父进程的信号处理方式。因为复制了父进程内存映像，所以信号捕捉函数的地址在子进程中才有意义。

## 不可靠的信号

早期UNIX版本V7，信号可能会丢失。一个信号发生，而进程未知。进程对信号的控制能力也很差，能捕捉信号或忽略它。 用户希望通知内核阻塞某个信号，发生时记住，进程准备好再通知。

存在的问题是进程每次接到信号对其处理时，随即将该信号动作重置为默认值。通过对每种信号只捕捉一次可以回避该点。

## 中断的系统调用

进程执行低速的系统调用，阻塞期间捕捉到一个信号，则该内核中的系统调用被中断不再继续执行。系统调用返回出错，errno设置为EINTR。应当唤醒阻塞的系统调用。

为支持该特性，系统调用分为低速系统调用和其他系统调用，低速系统调用可能使进程永远阻塞，包括：

1、如果某些类型文件数据（读管道、终端设备、网络设备）不存在，读操作可能会使调用者永远阻塞；

2、如果这些数据不能被相同的类型文件立即接受，则写操作可能会使调用者永远阻塞；

3、在某种条件发生之前打开某些类型文件，可能会发生阻塞（打开一个终端设备，需要先等待与之连接的调制解调器应答）

4、pause函数和wait函数

5、某些ioctl操作

6、某些进程间通信函数。

与磁盘I/O有关的系统调用。读写一个磁盘文件可能暂时阻塞调用者，除非发生硬件错误，I/O操作总会很快返回，并使调用者不再处于阻塞状态。

使用中断系统调用这种方法来处理：一个进程启动了读终端操作，而使用该终端设备的用户却长时间离开终端。

read系统调用已经接收并传送数据至应用程序缓冲区但尚未接收完全，被中断，可以被认为系统调用失败，或允许系统调用成功返回，返回值为接收到的数据量。errno设置为EINTR。write同理，System V派生的实现失败，BSD派生的成功。

与被中断的系统调用相关问题是必须显式地处理出错返回。

为帮助应用程序使其不必处理被中断的系统调用，被中断的系统调用自动重启动。包括，ioctl、read、readv、write、writev、wait和waitpid。

前五个对低速设备操作时才会被信号中断。wait和waitpid捕捉到信号总是被中断。

4.2BSD引入自动重启动，在于用户未知输入、输出设备是否低速。4.3BSD允许进程基于每个信号禁用此功能。

## 可重入函数

进程捕捉到信号并对其进行处理时，进程正在执行的正常指令序列就被信号处理程序临时中断，并首先执行该信号处理程序中的指令。如果从信号处理程序返回，则继续执行正常指令序列。

在信号处理程序中，不能判断捕捉到信号时进程执行到何处。如malloc执行过程捕捉到信号又调用malloc可能对进程造成破坏，malloc通常为它所分配的存储区维护一个链表。在执行getpwnam时又中断调用getpwnam，信息被覆盖。

信号处理程序中保证调用安全的函数。可重入函数，异步信号安全的。除了可重入外，信号处理操作期间，会阻塞任何会引起不一致的信号发送。

不可重入：已知使用静态数据结构；调用malloc或free；标准I/O函数。标准I/O库很多实现都以不可重入方式使用全局数据结构。

每个线程只有一个errno变量，信号处理程序可能会修改其原先值。在信号处理程序调用可重入函数前应保存errno，调用后恢复errno。

信号处理程序中调用一个非可重入函数，结果不可预知的。

## SIGCLD语义

Syetem V的信号名。

早期处理方式：

1、进程明确将该信号配置设置为`SIG_IGN`，则调用进程的子进程不产生僵尸进程。子进程在终止时，将其状态丢弃。调用进程随后调用wait函数则将阻塞直到所有子进程终止，该wait返回-1，errno设置为ECHILD。（SIGCLD默认配置是忽略，但不会使上述语义其作用，即`SIG_DFL` 默认是`SIG_IGN` 但是无效）。

2、SIGCLD设置为捕捉，内核立即检查是否有子进程准备好被等待，如果有则调用SIGCLD处理程序。

应当在调用wait取得子进程的终止状态后调用signal，仅当其他子进程终止，内核才会再次产生此种信号。

在Linux3.2.0和Solaris 10中 SIGCLD等同于SIGCHLD。

## 可靠信号术语和语义

产生：造成信号的事件发生时，为进程产生一个信号。一个信号产生时，内核通常在进程表中以某种形式设置一个标志。

递送：对信号采取了这种动作时（对信号处理），则向进程递送一个信号。在信号产生和递送之间的时间间隔，信号是未决的。

进程产生了一个阻塞的信号，且对该信号动作为系统默认动作或捕捉该信号，则为该进程将此信号保持为未决状态，直到进程解除信号阻塞或将此信号的动作更改为忽略。**进程在信号递送给它前仍可改变对该信号的动作。**

进程解除对某个信号的阻塞之前，信号发生了多次，允许递送该信号一次或多次，递送多次则称信号进行排队。大多数UNIX实现不对信号排队，只递送这种信号一次。

多个信号递送给一个进程，没有规定递送顺序，建议其他信号之前递送与进程当前状态有关的信号。

每个进程都有一个信号屏蔽字，规定了当前要阻塞递送到该进程的信号集。对于每种可能的信号，屏蔽字中有一位与之对应。

信号编号可能超过一个整型所包含的二进制位数，定义了`sigset_t`容纳一个信号集，存放信号屏蔽字。

## 函数kill和raise

kill函数将信号发送给进程或进程组，raise函数则允许进程向自身发送信号

```c
#include <signal.h>
int kill(pid_t pid, int signo);
int raise(int signo);
//成功返回0 出错返回-1
```

调用`raise(signo)`等价于调用`kill(getpid(), signo)`

kill的pid参数存在4种情况：

1、pid > 0 将该信号发送给进程ID为pid的进程。

2、pid == 0 将该信号发送给与发送进程属于同一进程组的所有进程，而且发送进程具有权限向这些进程发送信号。（所有进程不包括实现定义的系统进程集）。大多数UNIX系统，系统进程集包括内核进程和init

3、pid < 0 将该信号发送给其进程组ID等于pid绝对值，而且发送进程具有权限向其发送信号的所有进程。不包括系统进程集中的进程。

4、pid == -1 将该信号发送给发送进程有权限向它们发送信号的所有进程。不包括系统进程集中的进程

进程将信号发送给其他进程需要权限。超级用户可发送给任一进程。非超级用户，发送者实际用户ID或有效用户ID等于接受者实际用户ID或有效用户ID才行。 支持`_POSIX_SAVED_IDS`，检测接受者的保存设置用户ID。

signo参数为0，为空信号，kill仍执行正常的错误检查，但不发送信号。常被用来确定一个特定进程是否仍然存在。不存在kill返回-1，errno被设置为ESRCH。

进程ID可复用，现有特定ID进程不一定是需要的进程。

测试进程是否存在不是原子操作，kill返回测试结果时，存在的进程可能终止。

若调用kill为调用进程产生信号，而且此信号不被阻塞，则在kill返回之前，signo或某个其他未决的、非阻塞信号被传送至该进程。

## 函数alarm和pause

alarm设置定时器，将来某时刻该定时器会超时。超时产生SIGALRM信号。默认动作是终止调用该alarm进程。

```c
#include <unistd.h>
unsigned int alarm(unsigned int seconds);
//返回0或以前设置的闹钟时间的余留秒数。
```

参数seconds的值是产生信号SIGALRM需要经过的时钟秒数。时刻到达时，**内核**产生信号，进程调度的延迟，进程得到控制并处理该信号需要一个时间间隔。

每个进程只能有一个闹钟时间。调用alarm时，之前的未超时，则该闹钟时间的余留值作为本次alarm返回值。以前注册的闹钟时间被新值代替。

若以前注册尚未超时，本次调用seconds为0，则取消以前的闹钟时间，余留值作为返回。

SIGALRM默认动作是终止进程，大多数使用闹钟的进程捕捉此信号。若捕捉SIGALRM，则必须在调用alarm之前安装该信号的处理程序。先调用alarm，在安装处理程序前接收到该信号则进程终止。

```c
#include <unistd.h>
int pause(void);
//返回-1， errno设置为EINTR
```

调用进程挂起直至捕捉到一个信号。

只有执行了一个信号处理程序并从其返回时，pause才返回。此种情况pause返回-1。

## 信号集

表示多个信号——**信号集**的数据类型。

```c
#include <signal.h>
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signo);
int sigdelset(sigset_t *set, int signo);
//成功返回0 出错返回-1
int sigismember(const sigset_t *set, int signo);
//真1 假0
```

sigemptyset初始化由set指向的信号集，清除其中所有信号。sigfillset初始化，其包含set指向信号集的所有信号。所有应用程序在使用信号集前，需要初始化。这有C将不赋初值的外部变量和静态变量都初始化为0。

初始化信号集后可以在该信号集内增、删特定信号。sigaddset将一个信号加入信号集，sigdelset从信号集删除信号，对所有以信号集作为参数的函数，总是以信号集地址作为向其传送的参数。

## 函数sigprocmask

信号屏蔽字规定了进程当前阻塞不能递送给该进程的信号集。sigprocmask可以检测或更改

```c
#include <signal.h>
int sigprocmask(int how, const sigset_t *restrict set, sigset_t *restrict oset);
//成功返回0 出错返回-1
```

若oset为非空指针，进程的当前信号屏蔽字通过oset返回；若set是非空指针，how指示如何修改当前信号屏蔽字。

how包括`SIG_BLOCK`并集 ，添加阻塞`SIG_UNBLOCK`与set补集的交集，解除阻塞`SIG_SETMASK`更改阻塞，为set

set是空指针则不改变进程信号屏蔽字，how无意义。

sigprocmask后任何未决的、不再阻塞的信号，在sigprocmask返回前，至少将其中之一递送给该进程。

仅仅为单线程进程定义。

## 函数sigpending

返回一信号集，对于调用进程而言，其中各信号是阻塞不能递送的，是当前未决的。

```c
#include <signal.h>
int sigpending(sigset_t *set);
//成功返回0出错返回-1
```

## 函数sigaction

sigaction函数的功能是检查或修改（或检查并修改）与指定信号相关联的处理动作。

```c
#include <signal.h>
int sigaction(int signo, const struct sigaction *restrict act,
                struct sigaction *restrict oact);
//成功0 出错-1
```

signo为检测或修改的具体动作的信号编号，act指针非空则要修改其动作，oact指针非空，则oact指针返回该信号的上一个动作。

```c
struct sigaction{
    void (*sa_handler)(int); /* addr or signal handler */
                            /* or SIG_IGN, or SIG_DEL */
    sigset_t sa_mask;       /* additional signals to block */
    int sa_flags;           /* signal options, Figure 10.16 */
    /* alternative handler */
    void (*sa_sigaction)(int, siginfo_t *, void *);
};
```

更改信号动作，如果`sa_handler`包含一个信号捕捉函数的地址，则`sa_mask`字段说明了一个信号集，在调用该信号捕捉函数之前，这一信号集需加到进程的信号屏蔽字中。仅当从信号捕捉函数返回时再将进程的信号屏蔽字恢复为原先值。

信号处理程序被调用时，操作系统建立的新信号屏蔽字包括正被递送的信号。保证处理一个给定信号时，信号再次发生会被阻塞。

一旦对给定信号设置一个动作，在调用sigaction显式改变之前，设置一直有效。

`sig_sigaction`字段是一个替代的信号处理程序，在sigaction结构中使用`SA_SIGINFO`标志，则使用该信号处理程序。handler和sigaction字段实现可能使用同一存储区，所以应用只能一次使用这两字段中一个。

siginfo结构包含了信号产生原因的有关信息。

## 函数sigsetjmp和siglongjmp

信号处理程序中进行非局部转移时应当使用这两个函数

```c
#include <setjmp.h>
int sigsetjmp(sigjmp_buf env, int savemask);
//直接调用返回0，siglongjmp返回非0
void siglongjmp(sigjmp_buf env, int val);
```

sigsetjmp增加了一个参数，若savemask非0，sigsetjmp在env中保存进程的当前信号屏蔽字，调用siglongjmp时，非0 savemask的sigsetjmp调用中已经保存了env，则siglongjmp从其中回复保存的信号屏蔽字。

设置易失变量，调用sigsetjmp后将其设置为非0，仅当其非0时可以siglongjmp，使得jmpbuf尚未由sigsetjmp初始化时，防止调用信号处理程序。

`sig_atomic_t`写时不会被中断，所以在具有虚拟存储器的系统上变量不会跨越页边界，总是volatile修饰的，原因在于该变量将由main函数和异步执行信号处理程序访问。

siglongjmp恢复了由sigsetjmp保存的信号屏蔽字。

## 函数sigsuspend

更改进程的信号屏蔽字，可以阻塞信号或解除信号阻塞。

对一个信号解除阻塞，再以pause等待被阻塞信号的发生，这期间存在时间窗口。sigsuspend在一个原子操作中恢复信号屏蔽字，再使进程休眠。

```c
#include <signal.h>
int sigsuspend(const sigset_t *sigmask);
//返回-1 errno设置成EINTR
```

进程的信号屏蔽字设置为由sigmask指向的值，在捕捉到一个信号或发生了一个会终止该进程的信号之前，该进程被挂起。若捕捉到一个信号并从该信号处理程序返回，则sigsuspend返回并且该进程信号屏蔽字设置为调用sigsuspend之前的值。

sigsuspend另一种应用是等待一个信号处理程序设置一个全局变量。

用信号可实现父子进程间的同步。

## 函数abort

```c
#include <stdlib.h>
void abort(void);
```

使程序异常终止。信号SIGABRT发送给调用进程。等效于raise（SIGABRT）

若捕捉到此信号且相应处理程序返回，abort仍 不会返回到其调用者。abort不理会进程对此信号的阻塞和忽略。

进程捕捉SIGABRT的意图为在进程终止之前由其执行所需的清理操作。进程不在信号处理程序中终止自己，则其处理程序返回时abort终止该进程。

abort调用终止进程，对所有打开标准I/O流的效果与进程终止前对每个流调用fclose相同。

## 函数system

system应当忽略SIGINT和SIGQUIT，阻塞SIGCHLD直至waitpid获取子进程终止状态

system运行另一个程序是，父子进程两者不应都捕捉终端产生的两个信号：INT和QUIT。system的调用者在等待命令完成时应当忽略这两个信号。

### system返回值

是shell的终止状态，仅当shell本身异常终止，system返回值才报告一个异常终止。

## 函数sleep、nanosleep和clock_nanosleep

```c
#include <unistd.h>
unsigned int sleep(unsigned int seconds);
//返回0或者未休眠完的秒数。
```

此函数使调用进程被挂起直到满足下面两个条件之一：

1、过了seconds指定的墙上时钟时间。

2、调用进程捕捉到一个信号并从信号处理程序返回。

同alarm一样，实际返回时间由于其他系统活动可能比所要求的迟一些。

```c
#include <time.h>
int nanosleep(const struct timespec *reqtp, struct timespec *remtp);
//休眠到要求时间返回0，出错返回-1
```

提供了纳秒级的精度，函数挂起调用进程直至超时或者某个信号中断了该函数。reqtp指定秒和纳秒时间长度，信号中断休眠，进程未终止，remtp被设置为未休眠完的时间长度，可设置为NULL。

```c
#include <time.h>
int clock_nanosleep(clockid_t clock_id, int flags,
                    const struct timespec *reqtp, struct timespec *remtp);
//休眠要求值则返回0，出错返回错误码
```

多个系统时钟的引入，需要使用相对于特定时钟的延迟时间来挂起调用线程。

`clock_id`指定时钟类型，flags控制延迟是相对还是绝对，为0相对（休眠时间长度），`TIMER_ABSTIME`绝对（休眠到的特定时间）。

绝对时间使用 remtp没必要使用。

除了出错返回`clock_nanosleep(CLOCK_REALTIME, 0, reqtp, remtp)`和`nanosleep(reqtp,remtp)`效果相同。

绝对时间改善精度，相对休眠时间通常实际休眠时间长，处理器调度和抢占。

## 函数sigqueue

信号排队，通常一个信号带有一个位信息：信号本身。

使用排队信号必须做出以下几个操作：

1、使用sigaction函数安装信号处理程序时，指定`SA_SIGINFO`标志，未给出标志信号会延迟，信号是否进入队列取决于具体实现。

2、sigaction结构中`sa_sigaction`成员中提供信号处理程序。实现可能运行用户使用handler字段但不能获取sigqueue函数信息。

3、使用sigqueue函数发送信号。

```c
#include <signal.h>
int sigqueue(pid_t pid, int signo, const union sigval value);
//成功返回0 出错返回-1
```

sigqueue只能把信号发送给单个进程，使用value向信号处理程序传递整数和指针值，除此之外与kill类似。

信号不能被无限排队，达到一定数量会失败 errno设置为EAGAIN。

随着实时信号增强，引入了用于应用程序的独立信号集，信号编号在SIGRTMIN~SIGRTMAX之间，包括两限制值，这些信号默认行为是终止进程。

## 作业控制信号

6个与作业控制有关的信号

SIGCHLD 子进程停止或终止； SIGCONT 进程停止则使其继续运行； SIGSTOP 停止信号；SIGTSTP 交互式停止信号；SIGTTIN 后台进程组成员读控制终端；SIGTTOU 后台进程组成员写控制终端。

shell通常处理这些信号的所有工作。一个例外是管理终端的进程，如vi。用户要挂起它时，它需要能了解到这点用于恢复状态。

作业控制信号中有交互，4种停止信号都使该进程任一未决SIGCONT信号被丢弃。SIGCONT也使得任一未决停止信号被丢弃。

## 信号名和编号

信号编号和信号名之间进行映射，某些系统提供数组

`extern char *sys_siglist[];`

数组下标为编号，元素为指向信号名的指针。

```c
#include <signal.h>
void psignal(int signo, const char *msg);
```

msg通常是程序名，输出到标准错误文件，后面为冒号空格+信号的说明+换行符，msg为NULL则只有信号说明部分输出到标准错误文件，类似perror。

```c
#include <signal.h>
void psiginfo(const siginfo_t *info, const char *msg);
```

如果sigaction信号处理程序中有siginfo结构，使用psiginfo打印信号信息，工作方式与psignal类似。

```c
#include <string.h>
char *strsignal(int signo);
//返回指向描述该信号的字符串指针
```

只需要信号的字符描述部分，无需写到标准错误文件，使用strsignal函数 类似 strerror

```c
#include <signal.h>
int sig2str(int signo, char *str);
int str2sig(const char *str, int *signop);
//成功返回0 出错返回-1
```

将信号编号与信号名互相映射。

sig2str将编号翻译成字符串，结果存放在str指向的存储区，包括终止null字符。

str2sig函数将给定信号名翻译成信号编号。信号编号存放在signop指向的整型中。名字要么是不带SIG的信号名，要么是十进制信号编号字符串。

失败时不设置errno。
