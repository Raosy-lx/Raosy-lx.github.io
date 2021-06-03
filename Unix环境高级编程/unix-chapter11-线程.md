# 线程

相关的进程间可以存在一定的共享。

多个线程在单进程环境中执行多个任务。一个进程的所有线程都可以访问该进程的组成部分，如文件描述符和内存。

单个资源需多个用户共享，存在一致性问题，需要同步。

## 线程概念

单线程进程在某一时刻只能做一件事，多控制线程把进程设计成某一时刻能够做不止一件事，每个线程处理各自独立的任务，具有好处：

1、简化处理异步事件的代码。每个线程在进行事件处理时可采用同步编程模式。

2、多线程可访问相同存储地址空间和文件描述符，多个进程使用OS的复杂机制实现共享

3、有些问题可以分解来提高整个程序的吞吐量。单线程进程完成多任务需串行化，多线程相互独立的任务处理可以交叉进行。

4、交互程序使用多线程改善响应时间。

每个线程都包含有表示执行环境所必需的信息，包括：线程ID、一组寄存器值、栈、调度优先级和策略、信号屏蔽字、errno变量以及线程私有数据。

一个进程的所有信息对该进程内的线程都共享，包括可执行程序的代码、程序的全局内存和堆栈以及文件描述符。

## 线程标识

每个线程有线程ID，只在它所属的进程上下文中有意义。

```c
#include <pthread.h>
int pthread_equal(pthread_t tid1, pthread_t tid2);
//若相等返回非0值，否则返回0
```

比较两个线程ID是否相等，`pthread_t`用结构表示，不能用可移植的方式打印数据类型的值。

```c
#include <pthread.h>
pthread_t pthread_self(void);
//返回调用线程的线程ID
```

当线程需要识别以线程ID作为标识的数据结构时，`pthread_self`结合`pthread_equal`使用，使得工作线程只能处理标有自己线程ID的作业。

## 线程创建

```c
#include <pthread.h>
int pthread_create(pthread_t *restrict tidp,
                    const pthread_attr_t *restrict attr,
                    void *(*start_rtn)(void *), void *restrict arg);
//成功返回0，否则返回错误编号
```

创建新增的线程，成功返回时，新创建的线程ID被设置成tidp指向的内存单元。 attr参数用于定制不同的线程属性，新创建的线程从start_run函数的地址开始运行，该函数只有一个无类型指针参数arg，arg可能是结构，包含多个参数。

新创建线程和调用线程运行顺序不定，新创建的线程可以访问进程的地址空间，且继承调用线程的浮点环境和信号屏蔽字，但该线程的挂起信号集会被清除。

pthread调用失败返回错误码，每个线程提供errno的副本，线程中函数返回错误码不依赖随着函数执行不断变化的全局状态，可以限制错误范围在引起出错的函数中。

## 线程终止

默认的动作是终止进程，则发送到线程的信号会终止整个进程。

单个线程可以通过3种方式退出，在不终止整个进程的情况下停止它的控制流：

1、线程简单地从启动例程中返回，返回值是线程的退出码。

2、线程可以被同一进程中的其他线程取消。

3、线程调用`pthread_exit`

```c
#include <pthread.h>
void pthread_exit(void *rval_ptr);
```

`rval_ptr`是一个无类型指针，与传给启动例程的单个参数类似。进程中其他线程也可以通过调用`pthread_join`函数访问到这个指针。

```c
#include <pthread.h>
int pthread_join(pthread_t thread, void **rval_ptr);
//成功返回0，否则返回错误编号
```

调用线程将一直阻塞，直到指定线程调用`pthread_exit`、从启动例程返回或者被取消。线程简单从启动例程返回，rvalptr包含返回码，线程被取消，则rvalptr指定的内存单元被设置为`PTHREAD_CANCELED`。

当一个线程退出或简单地从启动例程返回，进程中其他线程可以通过join函数获得该线程的退出状态。

`pthread_create`和`pthread_exit`函数的无类型指针参数可传递的值不止一个，可以传递包含复杂信息的结构的地址。该结构使用的内存在调用者完成调用后必须仍然有效。

可使用全局结构或者用malloc函数分配结构，避免调用线程的栈或者线程在自己的栈区分配结构导致内存失效。

线程可通过调用`pthread_cancel`请求取消同一进程中的其他线程

```c
#include <pthread.h>
int pthread_cancel(pthread_t tid);
//成功返回0，否则返回错误编号
```

使tid标识的线程行为表现如同调用了参数为`PTHREAD_CANCELED`的`pthread_exit`函数。但是线程可以忽略取消或者控制如何被取消。仅仅提出请求并不等待线程终止。

线程可以安排它退出时需要调用的函数，即使用线程清理处理程序。一个线程可以建立多个清理处理程序，记录在栈，执行与注册顺序相反。

```c
#include <pthread.h>
void pthread_cleanup_push(void (*rtn)(void *), void *arg);
void pthread_cleanup_pop(int execute);
```

线程执行以下动作，清理函数rtn由push调度，调用时只有arg参数

※调用pthread_exit

※响应取消请求时

※用非零execute参数调用pop

execute为0，清理函数不被调用。

不管上述哪种情况，pop都将删除上次push调用建立的清理处理程序。

可以实现为宏，必须在与线程相同的作用域中以匹配对的形式使用。

默认情况下，线程的终止状态会保存直到对该线程调用join。如果线程已经被分离，线程的底层存储资源可以在线程终止时立即被收回。被分离后，不能用join等待终止状态。

```c
#include <pthread.h>
int pthread_detach(pthread_t tid);
//成功返回0 否则返回错误编号
```

用于分离线程。

## 线程同步

多线程共享相同内存，需确保每个线程看到的数据视图一致。对于**只读**或者线程内局部变量无一致性问题，对于**可写可读**的共享变量需要同步。

一个线程修改变量，其他线程读取变量可能不一致。同步读写需使用锁，同一时间只允许一个线程访问该变量。

两个或多个线程试图同一时间修改同一变量，需进行同步。变量增量操作：

1、从内存单元读入寄存器

2、在寄存器中对变量做增量操作

3、把新值写回内存单元

修改操作是原子操作则不存在竞争。若数据总是顺序一致出现，则不需要额外同步。多个线程观察不到数据的不一致时，操作就是顺序一致的。

顺序一致环境中，数据修改操作解释为运行线程的顺序操作步骤。

### 互斥量

pthread的互斥接口确保同一时间只有一个线程访问数据。访问共享资源前对互斥量加锁，访问完成解锁。加锁会导致再次访问互斥量线程被阻塞，直至当前线程释放该互斥锁。

必须所有线程都设计成遵守相同数据访问规则的，互斥机制才能正常工作。

```c
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t *restrict mutex,
                        const pthread_mutexattr_t *restrict attr);
int pthread_mutex_destory(pthread_mutex_t *mutex);
//成功返回0；否则返回错误编号
```

使用互斥变量前，需要对其初始化，可以设置成`PTHREAD_MUTEX_INITIALIZER`（只适用于静态分配的互斥量），也可以通过调用init函数初始化，若动态分配互斥量（用malloc），释放内存前需调用destory函数。

用默认属性初始化，需要把attr设为NULL。

```c
#include <pthread.h>
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
//成功返回0，否则返回错误编号
```

对互斥量加锁需要lock，解锁需unlock。对以加锁的互斥量lock的线程会被阻塞，线程不希望被阻塞，可以使用trylock，成功上锁返回0，否则返回EBUSY。

当一个以上线程需要访问动态分配的对象时，可以在对象中嵌入引用计数，确保在所有使用该对象的线程完成数据访问之前，该对象内存空间不会被释放。

### 避免死锁

线程试图对同一互斥量加锁两次，自身会陷入死锁状态。

拥有互斥量1的线程1试图锁住互斥量2，拥有互斥量2的线程2试图锁住互斥量1，交叉上锁产生死锁。

可以通过仔细控制互斥量加锁的顺序来避免死锁的发生。可能出现的死锁只会发生在一个线程试图锁住另一个线程以相反的顺序锁住的互斥量。

应用程序的结构使得对互斥量进行排序很困难，先释放占有的锁，过段时间再试，使用trylock接口可避免死锁，能获取锁则前进，不能则可释放已有锁，清理工作完成等待一段时间再重新试。

### 函数pthread_mutex_timedlock

```c
#include <pthread.h>
#include <time.h>
int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex,
                            const struct timespec *restrict tsptr);
//成功返回0，否则返回错误编号
```

线程试图获取一个已加锁的互斥量时，timedlock允许绑定线程阻塞时间。与lock函数基本等价，达到超时时间后，不会对互斥量加锁而是返回ETIMEDOUT

超时指定愿意等待的**绝对时间**，用秒和纳秒来描述。

### 读写锁

互斥量只有两种状态，加锁与不加锁；读写锁有三种状态：读模式下加锁状态，写模式下加锁状态，不加锁状态。

一次只有一个线程可以占有写模式的读写锁，多个线程可同时占有读模式的读写锁。

写加锁状态，被解锁前，其他加锁线程都被阻塞。读加锁状态，读模式对它加锁的线程都可以得到访问权，以写模式加锁线程都会阻塞，直到所有线程释放读锁。

通常，若读模式锁住时，写模式线程请求加锁，读写锁阻塞随后的读模式锁请求。

适用于对数据结构读的次数远大于写的情况，又称共享互斥锁。读模式锁住可以是共享模式锁住的，写模式锁住可以说成是以互斥模式锁住的。

读写锁在使用之前必须初始化，释放它们底层的内存前必须销毁。

```c
#include <pthread.h>
int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,
                        const pthread_rwlockattr_t *restrict attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
//成功返回0，否则返回错误编号
```

init初始化，attr为NULL，则读写锁有默认属性。

静态分配读写锁的初始化可用 PTHREAD_RWLOCK_INITIALIZER，释放锁占用内存前，需destroy锁init过程分配的资源。否则资源丢失

```c
#include <pthread.h>
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
//成功返回0， 否则返回错误编号
```

Single UNIX Specification定义了读写锁的条件版本

```c
#include <pthread.h>
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
// 成功返回0， 否则返回错误编号
```

可获取锁返回0， 否则返回EBUSY

作业队列中的增删改都是写锁，而查找则是读锁

### 带有超时的读写锁

```c
#include <pthread.h>
#include <time.h>
int pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rwlock,
                                const struct timespec *restrict tsptr);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rwlock,
                                const struct timespec *restrict tsptr);
// 成功返回0， 否则返回错误编号
```

与不计时版本类似，SUS中提供，避免陷入永久阻塞状态。获取锁返回0，否则超时到期，返回ETIMEDOUT，超时指定的是绝对时间。

### 条件变量

条件变量与互斥量一起使用时，允许线程以无竞争的方式等待特定的条件发生。

条件变量本身由互斥量保护。线程在改变条件状态之前必须首先锁住互斥量。互斥量必须在锁定以后才能计算条件。

```c
#include <pthread.h>
int pthread_cond_init(pthread_cond_t *restrict cond,
                        const pthread_condattr_t *restrict attr);
int pthread_cond_destroy(pthread_cond_t *cond);
//成功返回0，否则返回错误编号
```

静态分配初始化可用 `PTHREAD_COND_INITIALIZER`，动态分配则需init，释放条件变量底层的内存空间之前，可使用destroy对条件变量反初始化。

attr设置为null，以默认属性初始化条件变量。

```c
#include <pthread.h>
int pthread_cond_wait(pthread_cond_t *restrict cond,
                        pthread_mutex_t *restrict mutex);
int pthread_cond_timedwait(pthread_cond_t *restrict cond,
                            pthread_mutex_t *restrict mutex,
                            const struct timespec *restrict tsptr);
//成功返回0， 否则返回错误编号
```

使用wait等待条件变量变为真，给定时间内条件不能满足，返回错误码。

互斥量对条件进行保护，调用者把锁住的互斥量传给函数，函数然后自动把调用线程放到等待条件的线程列表上，对互斥量解锁。这就关闭了条件检查和线程进入休眠状态等待条件改变这两操作间的时间通道。wait返回时，互斥量再次被锁住。

timedwait，多了个超时，指定愿意等待的时长，为绝对数。

超时到期条件还没出现，timedwait将重新获取互斥量，然后返回ETIMEDOUT，从wait调用成功返回时，线程需要重新计算条件，因为另一个线程可能已经在运行并改变了条件。

```c
#include <pthread.h>
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
//成功返回0，否则返回错误编码
```

通知线程条件已经满足，signal至少能唤醒一个等待该条件的线程，broadcast唤醒所有等待该条件的线程。

给线程或条件发送信号，一定要在改变条件状态后。

条件是工作队列的状态，互斥量保护条件，把消息放队列需占有互斥量，给等待线程发送信号，不需要占有互斥量。

### 自旋锁

与互斥量类似，但不通过休眠阻塞进程，在获取锁之前一直处于忙等（自旋）阻塞状态。

可用于：锁被持有时间短，线程不希望在重新调度上花费太多成本。

当线程自旋等待变为可用时，CPU不能做其他事情。造成CPU浪费

在非抢占式内核中有用，可以阻塞中断。因为中断处理程序需要获取被加锁的自旋锁

用户层除非运行在不允许抢占的实时调度类才有用。运行在分时调度的用户层线程，时间片到期或者高优先级就绪变成可运行，均可以被取消调度。拥有自旋锁的进程会进入休眠。

自旋计数控制何时进入休眠。

```c
#include <pthread.h>
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_destroy(pthread_spinlock_t *lock);
//成功返回0 否则返回错误编号
```

初始化和反初始化，自旋锁只特有一个属性，只在支持线程进程共享同步选项的平台可用，pshared参数表示进程共享属性，表明自旋锁如何获取，`PTHREAD_PROCESS_SHARED`表示自旋锁可以被访问锁底层内存的属于不同进程的线程所获取。`PTHREAD_PROCESS_PRIVATE`则表示自旋锁只能被初始化该锁的进程内部线程所访问。

```c
#include <pthread.h>
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);
//成功返回0 否则返回错误编号
```

lock获取锁前一直自旋，trylock不能获取锁则返回EBUSY错误，调用者持有自旋锁情况下不要调用可能会进入休眠状态的函数，会浪费CPU资源，其他线程获取自旋锁需等待时间延长

### 屏障

用户协调多个线程并行工作的同步机制，屏障允许每个线程等待，直到所有的合作线程都到达某一点，然后从该点继续执行。

允许任意数量线程等待直到所有线程完成处理工作，线程不需要退出。所有线程达到屏障后可以接着工作。

```c
#include <pthread.h>
int pthread_barrier_init(pthread_barrier_t *restrict barrier,
                         const pthread_barrierattr_t *restrict attr,
                            unsigned int count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
//成功返回0，否则返回错误编号
```

初始化屏障，用count参数指定在允许所有线程继续运行前必须到达屏障的线程数目。attr指定屏障属性，使用init初始化需destroy反初始化。

```c
#include <pthread.h>
int pthread_barrier_wait(pthread_barrier_t *barrier);
//成功返回0或者PTHREAD_BARRIER_SERIAL_THREAD;否则返回错误编号
```

wait函数表明线程已完成工作准备等所有其他线程赶上来。

屏障计数未满足条件，线程进入休眠，若线程是最后一个调用wait的线程，则所有线程被唤醒。对于一个任意线程，其返回PTHREAD_BARRIER_SERIAL_THREAD，其他线程看到返回值为0，使一个线程可作为主线程，工作在其他所有线程已完成的工作结果上。

达到屏障计数，线程非阻塞，屏障可被重用。除非destroy再init 否则屏障计数不会改变。

线程的同步机制（互斥量、读写锁、条件变量、自旋锁和屏障）
