# 线程控制

控制线程行为，线程属性和同步原语属性

同一进程的多个线程之间保持数据的私有性，基于进程的系统调用如何与线程进行交互。

## 线程限制

通过sysconf函数可以查询限制，限制目的在于增强不同OS间实现的可移植性

包括 线程退出时OS实现试图销毁线程特定数据的最大次数；进程可创建键的最大数目；一个线程栈可用的最小字节数；进程可创建的最大线程数。

## 线程属性

pthread接口允许设置对象关联的不同属性，细调线程和同步对象的行为。管理这些属性的函数都遵循相同模式：

1、每个对象与它自己类型的属性对象相关联，一个属性对象可代表多个属性。属性对象对应用程序不透明，即应用程序不需要了解其内部结构细节。需提供函数管理属性对象。

2、有初始化函数，把属性设置为默认值。

3、销毁属性对象的函数，负责反初始化。

4、每个属性都有一个从属性对象中获取属性值的函数，可用参数指定属性值返回的内存单元

5、每个属性都有一个设置属性值的函数，属性值作为参数按值传递

init、destroy、get、set

```c
#include <pthread.h>
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
//成功返回0 失败返回错误编号
```

调用pthread create函数，可使用初始化的结构，修改线程默认属性。

destroy除了释放init资源和动态分配的内存空间，还会用无效的值初始化属性对象。

线程属性包括：线程分离状态属性detachstate、线程栈末尾警戒缓冲区大小（字节数）guardsize、线程栈的最低地址stackaddr、线程栈的最小长度stacksize

```c
#include <pthread.h>
int pthread_attr_getdetachstate(const pthread_attr_t *restrict attr,
                                int *detachstate);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int *detachstate);
//成功返回0， 否则返回错误编号
```

对线程终止状态不感兴趣，用detach分离在线程退出时回收资源。

创建线程时可设置detachstate，让线程一开始处于分离状态。可设置为`PTHREAD_CREATE_DETACHED`分离状态启动`PTHREAD_CREATE_JOINABLE`正常启动。

POSIX不一定支持线程栈属性，对于SUS中XSI选项的系统来说，支持线程栈属性是必需的。可以通过编译阶段特定符号检查支持的对应线程栈属性，或者运行阶段传给sysconf函数

```c
#include <pthread.h>
int pthread_attr_getstack(const pthread_attr_t *restrict attr,
                            void **restrict stackaddr,
                            size_t *restrict stacksize);
int pthread_attr_setstack(pthread_attr_t *attr,
                            void *stackaddr, size_t stacksize);
//成功返回0 否则返回错误编号
```

对线程栈属性进行管理。进程虚拟地址空间大小固定，进程中只有一个栈，大小不是问题，同样大小的虚拟地址空间被所有线程栈共享，可能超过可用的虚拟地址空间。线程数，线程函数内的自动变量数，调用函数涉及深栈帧等原因。

线程栈虚拟地址空间用完了，需malloc或mmap为可替代的栈分配空间，setstack改变新建线程的占位置，stackaddr参数指定的地址可用作线程栈的内存范围内最低可寻址地址。对齐且假设malloc和mmap所用虚拟地址范围和线程栈当前使用的虚拟地址范围不同。

stackaddr为栈最低内存地址，若栈从高向低增长，则为栈结尾位置，而不是开始位置。

```c
#include <pthread.h>
int pthread_attr_getstacksize(const pthread_attr_t *restrict attr,
                                size_t *restrict stacksize);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
//成功返回0 否则返回错误编号
```

获取或设置线程属性stacksize，希望改变默认栈大小而不处理线程栈的分配问题，可用setstacksize函数，参数不能小于`PTHREAD_STACK_MIN`。

```c
#include <pthread.h>
int pthread_attr_getguardsize(const pthread_attr_t *restrict attr,
                                size_t *restrict guardsize);
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
//成功返回0，否则返回错误编号
```

guardsize控制线程栈末尾后可用于避免栈溢出的扩展内存的大小，通常是系统页大小，可设置为0即不提供警戒缓冲区。修改stacksize，系统使栈警戒缓冲区机制无效，等同guardsize设置为0。

guardsize线程属性被修改，OS可把它取为页大小的整数倍，线程栈指针溢出到警戒区则应用程序通过信号接收到出错信息。 

## 同步属性

线程同步对象也有属性，自旋锁有进程共享属性。主要介绍互斥量属性、读写锁属性、条件变量属性和屏障属性

### 互斥量属性

```c
#include <pthread.h>
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
//成功返回0 否则返回错误编号
```

init使用默认的互斥量属性初始化attr，存在三个属性：**进程共享属性、健壮属性以及类型属性**。进程共享属性是可选的，不同平台不一定支持。

进程中，多线程可访问同一同步对象，此情况进程共享互斥量属性需设置为`PTHREAD_PROCESS_PRIVATE`。

多进程访问共享数据也需要同步。若互斥量设置为`PTHREAD_PROCESS_SHARED`，从多个彼此之间共享的内存数据块中分配的互斥量就可用于这些进程的同步。

```c
#include <pthread.h>
int pthread_mutexattr_getpshared(const pthread_mutexattr_t
                                        *restrict attr,
                                int *restrict pshared);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr,
                                int pshared);
//成功返回0， 否则返回错误编号
```

设置为私有时，允许pthread线程库提供更有效的互斥量实现，多线程应用程序中默认。多个进程共享多个互斥量情况下，pthread线程库可以限制开销较大的互斥量实现。

互斥量**健壮**属性与在多个进程间共享的互斥量有关。持有互斥量的进程终止时，需要解决互斥量状态恢复的问题。

```c
#include <pthread.h>
int pthread_mutexattr_getrobust(const pthread_mutexattr_t
                                        *restrict attr,
                                int *restrict robust);
int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr,
                                int robust);
//成功返回0， 否则返回错误编号
```

健壮属性取值有两种可能情况，默认是`PTHREAD_MUTEX_STALLED`，持有互斥量进程终止不采取特别措施。另一个取值是`PTHREAD_MUTEX_ROBUST`，导致线程调用`pthread_mutex_lock`获取锁，该锁被另一终止进程持有，线程阻塞且函数返回EOWNERDEAD而非0。应用程序通过特殊返回值可知，若有可能，需要恢复他们保护的互斥量。

使用健壮的互斥量， lock需要检查三个返回值，不需要恢复的成功，需要恢复的成功以及失败。

若应用状态无法恢复，线程对互斥量解锁后，该互斥量处于永久不可用状态。

```c
#include <pthread.h>
int pthread_mutex_consistent(pthread_mutex_t *mutex);
//成功返回0 否则返回错误编号
```

线程可指明与该互斥量相关的状态在互斥量解锁前是一致的。

若线程不用consistent而直接对互斥量解锁，其他试图获取该互斥量的阻塞线程会得到错误码ENOTRECOVERABLE，互斥量不再可用。

类型互斥量属性控制着互斥量的锁定特性，POSIX.1定义了4种类型：

`PTHREAD_MUTEX_NORMAL` 标准互斥量类型，不做任何特殊错误检查和死锁检测。

`PTHREAD_MUTEX_ERRORCHECK` 互斥量类型提供错误检查

`PTHREAD_MUTEX_RECURSIVE` 允许**同一线程**在互斥量解锁前对该互斥量多次加锁。递归互斥量维护锁的计数，解锁次数不等于加锁次数则不释放锁。

`PTHREAD_MUTEX_DEFAULT` 提供默认特性和行为，可自由映射到其他互斥量类型中的一种。

```c
#include <pthread.h>
int pthread_mutexattr_gettype(const pthread_mutexattr_t *restrict attr, int *restrict type);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
//成功返回0 否则返回错误编号
```

互斥量用于保护与条件变量关联的条件，改变条件时必须占有互斥量，使用递归互斥量不适用，条件永远不会得到满足。

单线程接口放到多线程环境，递归互斥量奏效。更改的是参数数据结构 而不是接口。

### 读写锁属性

```c
#include <pthread.h>
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
//成功返回0 否则返回错误编号
```

读写锁支持的唯一属性是进程共享属性，与互斥量的进程共享属性相同

```c
#include <pthread.h>
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t
                                        *restrict attr,
                                int *restrict pshared);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr,
                                int pshared);
//成功返回0， 否则返回错误编号
```

### 条件变量属性

SUS定义了条件变量的两个属性：进程共享属性和时钟属性

```c
#include <pthread.h>
int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);
//成功返回0 否则返回错误编号
```

支持进程共享属性

```c
#include <pthread.h>
int pthread_condattr_getpshared(const pthread_condattr_t
                                        *restrict attr,
                                int *restrict pshared);
int pthread_condattr_setpshared(pthread_condattr_t *attr,
                                int pshared);
//成功返回0， 否则返回错误编号
```

时钟属性控制计算timedwait函数的超时参数tsptr采用的是哪个时钟，合法值取自时钟ID

```c
#include <pthread.h>
int pthread_condattr_getclock(const pthread_condattr_t
                                        *restrict attr,
                                clockid_t *restrict clock_id);
int pthread_condattr_setclock(pthread_condattr_t *attr,
                                clockid_t clock_id);
//成功返回0， 否则返回错误编号
```

### 屏障属性

```c
#include <pthread.h>
int pthread_barrierattr_init(pthread_barrierattr_t *attr);
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
//成功返回0 否则返回错误编号
```

目前定义的屏障属性只有进程共享属性

```c
#include <pthread.h>
int pthread_barrierattr_getpshared(const pthread_barrierattr_t
                                        *restrict attr,
                                int *restrict pshared);
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr,
                                int pshared);
//成功返回0， 否则返回错误编号
```

## 重入

线程在遇到重入问题时与信号处理程序是类似的。多个控制线程在相同时间可能调用相同的函数。

如果一个函数在相同时间点可以被多个线程安全地调用，则其线程安全的。

返回数据放在静态缓冲区的函数并不是线程安全的。

一个函数对多个线程可重入，则该函数线程安全，但并不能说明对信号处理程序来说该函数也是可重入的。

若函数对异步信号处理程序的重入是安全的，则该函数异步信号安全。除了可重入，阻塞任何引起不一致的信号发送。

线程安全方式管理FILE对象的方法

```c
#include <stdio.h>
int ftrylockfile(FILE *fp);
//成功返回0 不能获取锁返回非0
void flockfile(FILE *fp);
void funlockfile(FILE *fp);
```

递归锁，可再次获取。

标准I/O以内部数据结构角度出发，线程安全方式实现，但把锁开放给应用程序也有用。可把多个标准I/O函数的调用组合成原子序列。

标准I/O例程获取各自的锁，一次一个字符的I/O有严重性能下降，可以用不加锁版本的基于字符的标准I/O例程

```c
#include <stdio.h>
int getchar_unlocked(void);
int getc_unlocked(FILE *fp);
//成功返回下一个字符，到文件尾或出错 EOF
int putchar_unlocked(int c);
int putc_unlocked(int c, FILE *fp);
//成功返回c 出错 EOF
```

除非被flockfile或trylock调用包围，否则尽量不调用上述4个函数，否则会导致不可预期的结果。

## 线程特定数据

也称线程私有数据，每个线程可以访问它自己单独的数据副本，不需要担心同步访问问题。

采用原因：

1、有时需维护基于每线程的数据。线程ID不一定是小而连续整数，不能作为线程数组的索引。且需额外保护防止某个线程数据与其他数据混淆。

2、提供了让基于进程的接口适应多线程环境的机制。errno

进程中所有线程都可访问这个进程的整个地址空间。除了使用寄存器以外，线程无法阻止另一线程的数据访问，包括线程私有数据。通过管理线程私有数据的函数来提高数据独立性

```c
#include <pthread.h>
int pthread_key_create(pthread_key_t *keyp, void (*destructor)(void *));
//成功返回0 否则返回错误编号
```

分配线程特定数据前，需创建与该数据关联的键，用于获取访问，键存在keyp指向的内存单元中，可被进程中所有线程使用，每个线程把这个键与不同的线程特定数据地址进行关联

pthreadkeycreate可以为该键关联一个可选择的析构函数。线程退出，**数据地址已被置为非空值，则析构函数被调用**。pthreadexit或线程执行返回，正常退出时，析构函数就会被调用。线程取消时，只有在最后的清理处理程序返回之后，析构函数才会被调用。

线程调用exit， _ exit，  _Exit或abort，或者其他非正常退出时，不会调用析构函数。

线程通常使用malloc为线程特定数据分配内存，析构函数通常释放已分配的内存。

线程可为线程特定数据分配多个键，每个键都可以有一个析构函数关联。可能不同也可能相同。

线程退出，线程特定数据的析构函数将按OS实现中定义顺序被调用，析构函数可能调用另一个函数，可能创建新线程特定数据，并把数据与当前键关联。所有析构函数调用完成，系统会检查是否还有非空线程特定数据值和键关联，有则再次调用析构函数。直到全空或达到最大次数的尝试。

```c
#include <pthread.h>
int pthread_key_delete(pthread_key_t key);
//成功返回0，否则返回错误编号
```

取消键与线程特定数据的关联，不会激活与键关联的析构函数，释放内存需采取额外步骤。

需确保分配的键不会由于在初始化阶段的竞争而发生变动。

```c
#include <pthread.h>
pthread_once_t initflag = PHTREAD_ONCE_INIT;
int pthread_once(pthread_once_t *initflag, void (*initfn)(void));
//成功返回0 否则返回错误编号
```

initflag必须为全局变量或静态变量，必须初始化。

```c
#include <pthread.h>
void *pthread_getspecific(pthread_key_t key);
//返回线程特定数据值，若没有值则返回NULL
int pthread_setspecific(pthread_key_t key, const void *value);
//成功返回0 否则返回错误编号
```

把键和线程特定数据关联，通过get返回值是否为空判断是否调用set。

## 取消选项

有两个线程属性并没有包含在`pthread_attr_t`结构中，它们是可取消状态和可取消类型。这两个属性影响线程在响应`pthread_cancel`函数调用时所呈现的行为。

可取消状态属性可为`PTHREAD_CANCEL_ENABLE`或者是`PTHREAD_CANCEL_DISABLE`。

```c
#include <pthread.h>
int pthread_setcancelstate(int state, int *oldstate);
//成功返回0， 否则返回错误编号
```

把可取消状态设置为state，并保留原状态在oldstate指向的内存单元，两步是一个原子操作

cancel并不等待线程终止，线程在取消请求发出后继续运行直至线程到达某个取消点，检查它是否被取消，若取消则按请求执行。

线程被设置为不可取消状态，cancel不会杀死线程，而被挂起，直到取消状态改变并到达下一个取消点，对所有挂起的取消请求进行处理。

```c
#include <pthread.h>
void pthread_testcancel(void);
```

可以调用testcancel函数添加自己的取消点，若某取消请求正处于挂起状态且被置为有效，线程就会被取消，若无效，则调用testcancel无效果。

默认取消类型为推迟取消

```c
#include <pthread.h>
int pthread_setcanceltype(int type, int *oldtype);
//成功返回0 否则返回错误编号
```

把取消类型设置为type（`PTHREADCANCEL_DEFERRED`和`PTHREAD_CANCEL_ASYNCHRONOUS`），原取消类型返回到oldtype指向的单元，异步取消和推迟取消不同，线程可以在任意时间撤销，不需取消点。

## 线程和信号

线程拥有自己的信号屏蔽字，但信号处理是进程中所有线程共享的。单个线程可以阻止某些信号，当某线程修改了某个给定信号的相关处理行为，所有线程都必须共享这个处理行为的改变。

若一个线程选择忽略某个给定信号，另一个线程可通过两种方式撤销上述线程的信号选择：恢复信号的默认处理行为，或者为信号设置一个新的信号处理程序。

进程中信号递送到单个线程的，若信号与硬件故障相关，信号一般被发送到引起事件的线程，而其他信号则被发送到任意一个线程。

```c
#include <signal.h>
int pthread_sigmask(int how, const sigset_t *restrict set,
                    sigset_t *restrict oset);
//成功返回0 否则返回错误编号
```

sigmask和sigpromash函数基本相同，工作在线程中，返回错误编号，set包含线程用于修改信号屏蔽字的信号集，oset不为空，则存储原信号屏蔽字，how分为3个值：

`SIG_BLOCK`，把信号集添加到线程信号屏蔽字 `SIG_SETMASK`，信号集替换线程信号屏蔽字

`SIG_UNBLOCK`，从线程信号屏蔽字中移除信号集。

set为NULL，oset非空，how被忽略，用于获取信号屏蔽字

```c
#include <signal.h>
int sigwait(const sigset_t *restrict set, int *restrict signop);
//成功返回0 否则返回错误编号
```

线程调用sigwait等待一个或多个信号的出现，set参数指定了线程等待的信号集。返回时，signop指向的整数包含发送信号的数量。

如果信号集中某个信号在sigwait调用时处于挂起状态，sigwait无阻塞返回，返回前，sigwait从进程中移除处于挂起等待状态的信号。

如果具体实现支持排队信号，且信号的多个实例被挂起，则sigwait只移除该信号的一个实例

线程调用sigwait前必须阻塞那些它正在等待的信号。sigwait函数会原子地取消信号集的阻塞状态，直到有新的信号被递送。返回前，sigwait将恢复线程的信号屏蔽字。

sigwait可以简化信号处理，允许把异步产生的信号用同步的方式处理。为防止信号中断线程，可将信号加入线程信号屏蔽字，安排专用线程处理信号。

多个线程在sigwait调用中因等待同一个信号而阻塞，信号递送只有一个线程可以从sigwait返回。信号被捕获，一个线程正在sigwait调用中等待同一信号，由OS实现决定以何种方式递送信号，可以让sigwait返回也可以激活信号处理程序，但不会同时发生。

```c
#include <signal.h>
int pthread_kill(pthread_t thread, int signo);
//成功返回0 否则返回错误编号
```

传0值的signo检查线程是否存在。信号默认处理动作是终止该进程，那把信号传递给某个线程会杀死整个进程。

闹钟定时器是进程资源，线程共享，进程中多线程不可能互不干扰地使用闹钟定时器。

## 线程和fork

线程调用fork，为子进程创建了整个进程地址空间的副本。子进程通过继承整个地址空间的副本，从父进程继承了每个互斥量、读写锁和条件变量的状态。如果父进程包含多个线程，子进程在fork后不是紧接着调用exec则需要清理锁状态。

子进程内部只存在一个线程，为调用fork线程的副本构成。父进程的线程占有锁，子进程将同样占有锁，但不知道它占有哪些锁需要释放哪些锁。

子进程从fork返回后马上调用其中一个exec函数，可以避免该问题。旧地址空间被丢弃，锁状态无关紧要。

多线程进程，为了避免不一致状态，在fork返回和子进程调用exec函数间，子进程只能调用异步信号安全的函数。

```c
#include <pthread.h>
int pthread_atfork(void (*prepare)(void), void (*parent)(void),
                    void (*child)(void));
//成功返回0 否则返回错误编号
```

清除锁状态，prepare由父进程在fork创建子进程前调用，获取父进程定义的所有锁。

parent在fork创建子进程后、返回之前在父进程上下文调用，对prepare获取的锁进行解锁

child在fork返回之前在子进程上下文中调用，释放prepare获取的锁。

fork存在写时复制，所以在prepare获取锁时，父子进程内存内容开始时相同，释放锁时，新的内存分配给子进程，父进程内存内容复制到子进程的内存中，父进程对其所有锁解锁，子进程对其锁副本加解锁。事件序列：

父进程获取所有锁、子进程获取所有锁、父进程释放它的锁、子进程释放它的锁。

可多次调用atfork函数设置多套fork处理程序，不需使用其中某个处理程序，给特定处理程序参数传入空指针即可。处理程序的调用顺序并不相同，parent和child fork处理程序是以注册时的顺序进行调用，而prepare调用顺序与注册时顺序相反。

模块A调用模块B函数，锁层次是A在B前，则B必须在A前设置fork处理程序，父进程调用fork时：

1、调用A的prepare fork处理程序获取A所有锁

2、调用B的prepare fork处理程序获取B所有锁

3、创建子进程

4、调用B的child fork处理程序释放子进程模块B所有锁

5、调用A的child fork处理程序释放子进程模块A所有锁

6、fork函数返回到子进程

7、调用模块B的parent fork处理程序释放父进程模块B所有锁

8、调用模块A的parent fork处理程序释放父进程模块A所有锁

9、fork函数返回到父进程

若锁是嵌入到条件变量中，调用fork后不能使用条件变量。

atfork存在一些不足，在有限情况下可用，不足之处：

1、没有很好办法对较为复杂的同步对象（条件变量或屏障）进行状态的重新初始化

2、某些错误检查的互斥量在child fork处理程序试图对被父进程加锁的互斥量解锁时产生错误

3、递归互斥量不能再child fork处理程序中清理，无法确定该互斥量被加锁的次数

4、若子进程只允许调用异步信号安全的函数，child fork处理程序不能清理同步对象，操作清理的函数非异步安全的。同步对象在fork时可能处于中间状态，除非同步对象处于一致状态，否则无法被清理

5、应用程序在信号处理程序调用fork，则atfork注册的fork处理程序只能调用异步信号安全的函数，否则结果未定义。





## 线程和I/O

pread和pwrite在多线程环境下有用，进程中所有线程共享相同的文件描述符。

pread使偏移量设定和数据的读取成为原子操作，把lseek+read可能造成的线程同步问题解决。 pwrite解决并发线程对同一文件进行写操作的问题。
