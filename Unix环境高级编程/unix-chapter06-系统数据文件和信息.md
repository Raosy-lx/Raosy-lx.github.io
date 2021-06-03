# 系统数据文件和信息

UNIX系统正常运作需要大量与系统相关数据文件，如口令文件/etc/passwd 组文件 /etc/group，用户登录UNIX系统，及每次执行ls -l 都需要使用口令文件

数据文件均为ASCII文本文件，使用标准I/O库读取。 较大系统顺序扫描口令文件费时

## 口令文件

又称用户数据库，这些字段包含在<pwd.h>中定义的passwd结构中。

/etc/passwd 是ASCII文件 每一行为：

`用户名：加密口令：用户ID：组ID：注释字段：初始工作目录：初始shell`

`root: root: 0: 0: root: /root: /bin/bash`

空白注释无影响 加密口令字段为空意味用户无口令。

默认shell /bin/sh。

组织特定用户登录系统，shell除了/dev/null 也可以 /bin/false 以非0状态终止，或者 /bin/true 以成功状态终止。 某些系统提供nologin。

nobody用户名目的在于任何人可登录系统 但用户ID和组ID 65534 不提供权限，只能访问人人可读写的文件。

finger 支持打印注释字段中附加信息 `finger -p user_name`

vipw可以编辑口令文件

```c
#include <pwd.h>
struct passwd *getpwuid(uid_t uid);
struct passwd *getpwnam(const char *name)
//成功返回指针， 出错返回NULL
```

获取口令文件项，ls使用getpwuid，将i节点中数字用户ID映射为用户登录名，login使用getpwnam。

passwd 通常是函数内部的静态变量，调用任一函数，其内容被重写。

```c
#include <pwd.h>
struct passwd *getpwent(void); //成功返回指针，失败或到文件尾返回NULL
void setpwent(void);
void endpwent(void);
```

getpwent 返回口令文件下一项，首次调用，打开它所使用的各个文件。 安排顺序无要求，有些系统使用散列算法。

setpwent 反绕使用文件 `SEEK_SET` endpwent 关闭这些文件。

getpwent后需要endpwent因为 getpwent只知道打开 不知道关闭。

getpwuid和getpwnam完成后也应使用endpwent

## 阴影口令

加密口令是经单向加密算法处理过的用户口令副本。算法为单向的

64字符集 a-z+A-Z+0-9+./ 中产生13个可打印字符； MD5或SHA-1算法对口令加密，产生更长的加密口令字符串。

用户口令往往非随机，实验为得到一份口令文件，用试探方法猜测口令。

为使企图猜测口令的人难以获得加密口令，某些系统将加密口令存放在阴影口令文件中，该文件至少要包含用户名和加密口令。

阴影口令文件不应是一般用户可读，只有少数几个程序如 login 和 passwd 需要访问加密口令，这些程序通常是设置用户ID 为 root的程序。这样/etc/passwd 可由普通用户读取。

```c
#include <shadow.h>
struct spwd *getspnam(const char *name);
struct spwd *getspent(void);
//成功返回指针 失败 NULL
void setspent(void);
void endspent(void);
```

## 组文件

字段存放在<grp.h>所定义的group结构中， 组名：加密口令：组ID：指向各用户名指针的数组 `char ** gr_mem`。该数组以null指针结尾。

```c
#include <grp.h>
struct group *getgrgid(gid_t gid);
struct group *getgrnam(const char *name);
//成功返回指针；出错返回NULL
```

查看组名或者组ID，返回静态变量指针。

```c
#include <grp.h>
struct group *getgrent(void);
//成功返回指针，出错或到文件尾返回NULL 
void setgrent(void);
void endgrent(void);
```

## 附属组ID

V7 中用户只属于一个组，可用newgrp更改，成功执行，实际组ID变为新组ID。执行不带任何参数的newgrp，可返回到原来组。

附属组ID，用户不仅可属于口令文件记录项的组ID对应组，还可属于最多16个另外的组，权限检查，将有效组ID和所有附属组ID均与文件组ID比较。

```c
#include <unistd.h>
int getgroups(int gidsetsize, gid_t grouplist[]);
//成功返回附属组ID数，失败返回-1
#include <grp.h> /*Linux*/
#include <unistd.h> /*FreeBSD, Mac OS X, Solaris*/
int setgroups(int ngroups, const gid_t grouplist[]);
#include <grp.h> /*Linux and Solaris*/
#include <unistd.h>/*FreeBSD, Mac OS X*/
int initgroups(const char *username, gid_t basegid);
//成功返回0 出错-1
```

getgroups将进程所属用户的各附属组ID填写到数组grouplist中，ID数最多为gidsetsize个，实际填写到的数量返回。

gidsetsize为0则返回附属组ID数，不对数组grouplist修改。

setgroups由超级用户调用来为调用进程设置附属组ID表，ngroups说明数组的元素数，不能大于`NGROUPS_MAX`。

通常只有initgroups调用setgroups，initgroups读整个组文件，对username确定其组成员关系，再用setgroups初始化附属组ID表。 basegid为username在口令文件中的组ID。

login在用户登录时调用initgroups。

## 实现区别

Linux Solaris支持阴影口令文件， FreeBSD和Mac OS X以不同方式存储加密口令字。

即没有/etc/passwd，FreeBSD中阴影口令是/etc/master.passwd。可用特殊命令编辑该文件，会从阴影口令文件产生/etc/passwd的一个副本。/etc/pwd.db是 /etc/passwd的散列副本，/etc/spwd.db是 /etc/master.passwd的散列副本。为大型安装的系统提供更好性能。

Mac OS X只在单用户模式下使用上述两种阴影口令，多用户模式采用目录服务守护进程。

Linux和Solaris对阴影口令存在细微差别，阴影口令中整型S 为int linux为long int；账户-不活动字段 S为自用户上次登录后到下次账户自动失效间的天数，Linux为达到最大口令年龄剩余天数。

NIS网络信息服务+轻量级目录访问协议LDAP， 客户端系统联系服务器查看用户和组信息

## 其他数据文件

记录网络服务器提供服务 /etc/services 记录协议信息 /etc/protocols 记录网络信息的数据文件 /etc/networks。

一般情况下对于每个数据文件至少有3个函数

1、get函数 读下一个记录，或者打开文件，通常返回指向结构的指针。达到文件尾端返回NULL，大多数返回指向静态存储类结构的指针，若要保存需复制。

2、set函数 打开相应数据文件（若未打开），反绕文件。

3、end函数 关闭文件。

如果数据文件支持某种形式键搜索，比如 getpwnam getpwuid

## 等级账户记录

UNIX提供两数据文件，utmp记录当前登录到系统的各个用户，wtmp文件跟踪各个登录和注销事件。 V7中，每次写入这两个文件中的是包含结构的一个二进制记录。

```c
struct utmp{
    char ut_line[8]; //tty line: "ttyh0"
    char ut_name[8]; //login name
    long ut_time; //seconds since epoch
}
```

登录时 login填写此类结构并写入utmp，同时加入wtmp。注销时，init进程将utmp相应记录擦除（各个字段赋值null），将新记录添写到wtmp。在wtmp中 `ut_name`字段清0， 系统再启动以及更改系统时间和日期前后，都在wtmp文件追加写特殊记录项。

who程序读取utmp文件 以可读格式打印其内容， last则读wtmp文件并打印，大多数UNIX版本仍然提供utmp和wtmp文件，扩充了结构包含信息量。

## 系统标识

```c
#include <sys/utsname.h>
int uname(struct utsname *name);
//成功返回非负值，出错返回-1
```

通过传递utsname结构地址， 此函数填写该结构

```c
struct utsname{
    char sysname[]; //name of operating system
    char nodename[]; //name of this node
    char release[]; //current release of operating system
    char version[]; //current version of this release
    char machine[]; //name of hardware type
}
```

每个字符串均以null字节结尾，utsname结构信息通常用uname打印。

```c
#include <unistd.h>
int gethostname(char *name, int namelen);
//成功返回0 出错-1
```

返回主机名，通常为TCP/IP网络上主机名，namelen参数指定name缓冲区长度，空间足够以null结尾，无足够空间则未说明是否以null结尾

最大主机名长度由 `HOST_NAME_MAX`限定。

宿主机联接到TCP/IP，主机名为该主机完整域名。 hostname命令用于获取和设置主机名。超级用户用sethostname设置主机名。

## 时间和日期例程

```c
#include <time.h>
time_t time(time_t *calptr); //成功返回时间，出错-1
```

参数非空，时间值也存放在calptr中。 返回当前时间和日期。

多个系统时钟，时钟类型 `CLOCK_REALTIME`, `CLOCK_MONOTONIC`（不带负跳数的实时系统时间）, `CLOCK_PROCESS_CPUTIME_ID`, `CLOCK_THREAD_PROCESS_ID`

```c
#include <sys/time.h>
int clock_gettime(clockid_t clock_id, struct timespec *tsp);
//成功返回0 失败-1
```

`CLOCK_REALTIME`与 time函数类似功能，系统支持高精度时间值，可能比time更高精度。

```c
#include <sys/time.h>
int clock_getres(clockid_t clock_id, struct timespec *tsp);
//成功返回0 失败-1
```

clock_getres把参数tsp指向的timespec结构初始化为与clock_id参数对应的时钟精度。

```c
#include <sys/time.h>
int clock_settime(clockid_t clock_id, const struct timespec *tsp);
//成功返回0 失败-1
```

对特定的时钟设置时间，需要适当特权更改时钟值。

System V 的stime BSD的settimeofday 

SUSv4 指定gettimeofday提供更高精度（微秒级）

```c
#include <sys/time.h>
int gettimeofday(struct timeval *restrict tp, void *restrict tzp);
//返回0 
```

tzp唯一合法值为NULL，可能说明时区，依实现，gettimeofday函数以距特定时间 1970.1.1. 0.0.0 秒数方式将时间存放在tp指向的timeval结构中，该结构表示当前时间秒+微妙

取得秒数整型时间值，需要分解成时间结构，并生成人可读时间日期。 localtime、mktime、strftime

localtime和gmtime将日历时间转换为分解时间并存在tm结构中

```c
struct tm{
    int tm_sec;//0-60
    int tm_min;
    int tm_hour;
    int tm_mday;//month day 1-31
    int tm_mon;
    int tm_year;
    int tm_wday;//weekend day 0-6
    int tm_yday;//year day 0-365
    int tm_isdst;//daylight saving time flag <0 0 >0
}
```

秒可以超过59，在于存在润秒，夏令时生效则标志为正，非夏令时为0，不可用为负

```c
#include <time.h>
struct tm *gmtime(const time_t *calptr);
struct tm *localtime(const time_t *calptr);
//成功返回指向tm的指针，出错返回NULL
```

localtime和gmtime区别，localtime转换成本地时间（时区+夏令时标志），gmtime为协调统一时间。

```c
#include <time.h>
time_t mktime(struct tm *tmptr);
//成功返回日历，出错返回-1
```

mktime以本地时间年月日等作参数，转换成time_t值。

```c
#include <time.h>
size_t strftime(char *restrict buf, size_t maxsize,
                 const char *restrict format,
                  const struct tm *restrict tmptr);
size_t strftime_l(char *restrict buf, size_t maxsize,
                 const char *restrict format,
                  const struct tm *restrict tmptr, locale_t locale);
//有空间返回存入数组字符数，否则为0
```

类似printf时间值函数，strftime_l允许调用者将区域设置为参数，其余两者相同，strftime使用TZ环境变量指定的区域，tmptr为需要格式化时间值，存放在maxsize字符长度的buf中，长度足够则以null结尾，返回不包含null的字符数，否则为0

format控制时间值格式，%XXX， 与printf类似。

%U相应日期在该年所属周数，包含该年第一个周日为第一周；%W包含第一个周一的周为第一周；%V，包含1.1日的周包含新年天数大于4，为第一周，否则为上年的最后一周。周一为每周第一天。

strptime是strftime的反过来版本，将字符串时间转换为分解时间。

```c
#include <time.h>
char *strptime(const char *restrict buf, const char *restrict format,
                  struct tm *restrict tmptr);
//返回上次解析的字符的下一个字符的指针，否则为NULL
```

![](C:\Users\asuss\AppData\Roaming\marktext\images\2021-05-12-13-11-32-image.png)

localtime mktime strftime 定义TZ则使用TZ时区，TZ为空串则为UTC时
