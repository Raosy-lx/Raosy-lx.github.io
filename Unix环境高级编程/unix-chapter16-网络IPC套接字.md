# 网络IPC: 套接字

管道、FIFO、消息队列、信号量、共享存储，用于同一台计算机上运行的进程间的通信。

不同计算机通过网络相连，其进程间通信：网络进程间通信。

套接字网络进程间通信接口，进程用该接口与其他进程通信，既可用于计算机间通信，也可用于计算机内通信。可使用多种网络协议，本章限制：TCP/IP协议栈。

## 套接字描述符

通信端点的抽象。通过套接字描述符访问。

```c
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
//成功返回套接字描述符，出错返回-1
```

创建一个套接字，doaim确定通信特性，包括地址格式。`AF_INET`IPv4因特网域，`AF_INET6`IPv6，`AF_UNIX`UNIX域，`AF_UPSPEC`未指定。AF指地址族address family。

type确定套接字类型，进一步确定通信特征。`SOCK_DGRAM`定长，无连接，不可靠报文传递，`SOCK_RAW`IP协议的数据报接口，`SOCK_SEQPACKET`定长，有序，可靠，面向连接的报文传递，`SOCK_STREAM`有序、可靠、双向、面向连接的字节流。

protocol通常是0，为给定域和套接字类型选择默认协议。若支持多个协议，protocol指定特定协议。`AF_INET`中，STREAM为TCP，DGRAM为UDP。

`IPPROTO_IP`IPv4网际协议

`IPPROTO_IPV6`IPv6网际协议

`IPPROTO_ICMP`ICMP协议，网际控制报文协议

`IPPROTO_RAW`原始IP数据包协议

`IPPROTO_TCP`传输控制协议

`IPPROTO_UDP`用户数据报协议

对于DGRAM数据报接口，两个对等进程间通信不需要逻辑连接。只需要向对等进程所使用套接字送出一个报文。STREAM要求交换数据前建立一个逻辑连接。

数据报是自包含报文，相反，面向连接的协议通信，为端到端的通信链路。

STREAM提供字节流服务，应用程序分辨不出报文的界限。也许不会返回所有由发送进程所写的字节数，可能需若干次调用获取发来的所有数据。

`SOCK_SEQPACKET`和`SOCK_STREAM`类似，只是基于报文的服务而不是字节流服务。接收数据量与发送一致。SCTP流控制传输协议提供了因特网域上顺序数据包服务。

`SOCK_RAW`提供一个数据报接口，直接访问网络层，应用程序负责构造自己的协议头部。创建原始套接字需要超级用户特权。

socket调用与open类似，均可获得用于I/O的fd，不需要使用则close关闭，且释放fd。

不是所有参数为fd的函数可接受套接字描述符，如lseek不行，套接字不支持文件偏移量的概念

```c
#include <sys/socket.h>
int shutdown(int sockfd, int how);
//成功返回0， 出错返回-1
```

套接字是双向的，可采用shutdown禁止一个套接字的I/O。

how是`SHUT_RD`无法使用套接字读取数据，是`SHUT_WR`则无法使用套接字发送数据。`SHUT_RDWR`无法通过套接字读写数据。

close和shutdown区别在于，最后一个活动引用关闭时，close才释放网络端点，即dup一个套接字，关闭最后一个引用的fd才释放套接字，shutdown允许使一个套接字处于不活跃状态，和引用它的fd数目无关。可以很方便地关闭传输方向。

## 寻址

标识一个目标通信进程，标识由两部分组成，一部分是计算机的网络地址，用于标识计算机，另一部分是端口号，标识特定进程。

### 字节序

同台计算机进程通信一般不考虑，是处理器架构特性，用于指示数据类型内部的字节排序。大小端问题，处理器架构支持大端字节序，最大字节地址出现在最低有效字节上，小端字节序相反。最高有效字节MSB总是在左边，最低有效字节LSB总是在右边。

如0x04030201 32位整数赋值，MSB包含4，LSB包含1，小端则cp[0]包含1，cp[3]包含4，大端字节序处理器上，cp[0]包含4，cp[3]包含1。

网络协议指定了字节序，异构计算机系统可交换协议信息而不被字节序混淆。TCP/IP协议栈使用大端字节序，应用程序交换格式化数据时，字节序问题会出现。TCP/IP,地址用网络字节序表示，应用程序需要在处理器字节序和网络字节序间转换。

```c
#include <arpa/inet.h>
uint32_t htonl(uint32_t hostint32);
//返回以网络字节序表示的32位整数
uint16_t htons(uint16_t hostint16);
//返回以网络字节序表示的16位整数
uint32_t ntohl(uint32_t netint32);
//返回以主机字节序表示的32位整数
uint16_t ntohs(uint16_t netint16);
//返回以主机字节序表示的16位整数
```

处理器字节序和网络字节序之间实施转换的函数

### 地址格式

地址标识特定通信域的套接字端点。地址被强制转换成一个通用地址结构sockaddr

```c
struct sockaddr{
    sa_family_t   sa_family; /* address family */
    char          sa_data[]; /* variable-length address */
    ...
};
```

套接字实现可以自由地添加额外的成员并且定义sa_data成员的大小。

`AF_INET`域中，地址用sockaddr_in表示：

```c
struct in_addr{
    in_addr_t    s_addr; /* IPv4 address */
};


struct sockaddr_in{
    sa_family_t    sin_family; /* address family */
    in_port_t      sin_port;   /* port number */
    struct in_addr sin_addr;   /* IPv4 address */
};
```

与`AF_INET`类似，IPv6因特网域套接字地址用sockaddr_in6表示

```c
struct in6_addr{
    uint8_t    s6_addr[16]; /* IPv6 address */
};


struct sockaddr_in6{
    sa_family_t     sin6_family; /* address family */
    in_port_t       sin6_port;   /* port number */
    uint32_t        sin6_flowinfo;   /* traffic class and flow info */
    struct in6_addr sin6_addr;   /* IPv6 address */
    uint32_t        sin6_scope_id; /*set of interfaces for scope */
};
```

尽管v4和v6地址结构差距大，均被强制转换成sockaddr结构输入套接字例程。

```c
#include <arpa/inet.h>
const char *inet_ntop(int domain, const void *restrict addr,
                        char *restrict str, socklen_t size);
//成功返回地址字符串指针；出错返回NULL
int inet_pton(int domain, const char *restrict str,
                void *restrict addr);
//成功返回1，格式无效返回0，出错返回-1
```

ntop将网络字节序的二进制地址转换成文本字符串格式，pton将文本字符串格式转换成网络字节序的二进制地址。domain仅仅支持`AF_INET`和`AF_INET6`。

ntop，size指定str大小。`INET_ADDRSTRLEN`定义足够大的空间存放一个IPv4文本字符串，`INET6_ADDRSTRLEN`则是IPv6。对于pton，`AF_INET`则addr需足够大空间存放32位地址，`AF_INET6`则需存放一个128位地址。

### 地址查询

理想情况，应用程序不需要了解一个套接字地址的内部结构。

BSD网络软件提供了访问网络配置信息的接口。

函数返回的网络配置信息被存放在许多地方。静态文件（/etc/hosts 和 /etc/services），也可由名字服务管理，域名系统DNS或网络信息服务NIS。

```c
#include <netdb.h>
struct hostent *gethostent(void);
//成功返回指针，出错返回NULL
void sethostent(int stayopen);
void endhostent(void);
```

gethostent，找到给定计算机系统的主机信息。主机数据库文件没打开，则get打开，返回文件中的下一个条目。set会打开文件，若文件已经被打开则回绕。stayopen设置非0则调用get后文件依然打开。end关闭文件。

gethostent返回指向hostent结构的指针，可能包含一个静态的数据缓冲区，每次调用缓冲区被覆盖。

```c
struct hostent{
    char    *h_name;        /* name of host */
    char   **h_aliases;     /* pointer to alternate host name array */
    int      h_addrtype;    /* address type */
    int      h_length;      /* length in bytes of address */
    char   **h_addr_list;   /* pointer to array of network addresses */
    ...
};
```

返回的地址采用网络字节序。

```c
#include <netdb.h>
struct netent *getnetbyaddr(uint32_t net, int type);
struct netent *getnetbyname(const char *name);
struct netent *getnetent(void);
//成功返回指针 出错返回NULL
void setnetent(int stayopen);
void endnetent(void);
```

与获得主机信息类似，获得网络名字和网络编号。netent结构至少包含字段：

```c
struct netent{
    char    *n_name;        /* network name */
    char   **n_aliases;     /* alternate network name array pointer */
    int      n_addrtype;    /* address type */
    uint32_t n_net;         /* network number */
};
```

网络编号按照网络字节序返回，地址类型是地址族常量之一，如`AF_INET`。

```c
#include <netdb.h>
struct protoent *getprotobyname(const char *name);
struct protoent *getprotobynumber(int proto);
struct protoent *getprotoent(void);
//成功返回指针，出错返回NULL
void setprotoent(int stayopen);
void endprotoent(void);
```

protoent结构至少包含成员：

```c
struct protoent{
    char    *p_name;        /* protocol name */
    char   **p_aliases;     /* pointer to altername protocol name array */
    int      p_proto;       /* protocol number */
    ...
};
```

服务由地址的端口号部分表示，每个服务由一个唯一的众所周知的端口号来支持。用getservbyname将一个服务名映射到一个端口号，用getservbyport将一个端口号映射到一个服务名，getservent顺序扫描服务数据库。

```c
#include <netdb.h>
struct servent *getservbyname(const char *name, const char *proto);
struct servent *getservbyport(int port, const char *proto);
struct servent *getservent(void);
//成功返回指针，出错返回NULL
void setservent(int stayopen);
void endservent(void);
```

servent结构至少包含

```c
struct servent{
    char    *s_name;        /* service name */
    char   **s_aliases;     /* pointer to alternate service name array */
    int      s_port;        /* port number */
    char    *s_proto;       /* name of protocol */
    ...
};
```

POSIX.1允许一个应用程序将一个主机名和一个服务名映射到一个地址。或者反之。

```c
#include <sys/socket.h>
#include <netdb.h>
int getaddrinfo(const char *restrict host,
                const char *restrict service,
                const struct addrinfo *restrict hint,
                struct addrinfo **restrict res);
//成功返回0 出错返回非0错误码
void freeaddrinfo(struct addrinfo *ai);
```

getaddrinfo允许将一个主机名和服务名映射到一个地址。

需要提供主机名、服务名，或者两者都提供。提供一个则另一个必须是空指针。主机名可以是一个节点名或点分格式的主机地址。

返回一个链表结构 addrinfo。可以通过freeaddrinfo来释放一个或多个结构，取决于`ai_next`字段链接起来的结构有多少。

addrinfo结构定义至少包含以下成员。

```c
struct addrinfo{
    int              ai_flags;     /* customize behavior */
    int              ai_family;    /* address family */
    int              ai_socktype;  /* socket type */
    int              ai_protocol;  /* protocol */
    socklen_t        ai_addrlen;   /* length in bytes of address */
    struct sockaddr *ai_addr;      /* address */
    char            *ai_canonname; /* canonical name of host */
    struct addrinfo *ai_next;      /* next in list */
    ...
};
```

hint来选择符合特定条件的地址。hint是用于过滤地址的模板，包括 family、flags、protocol、socktype字段。剩余整数字段设置为0，指针字段为空。

`ai_flags`标志定义如何处理地址和名字。

```c
#include <netdb.h>
const char *gai_strerror(int error);
//返回指向描述错误的字符串的指针。
```

getaddrinfo失败，不能用perror和strerror，需调用`gai_strerror`将错误码转换成错误消息。

```c
#include <sys/socket.h>
#include <netdb.h>
int getnameinfo(const struct sockaddr *restrict addr, socklen_t alen,
                char *restrict host, socklen_t hostlen,
                char *restrict service, socklen_t servlen, int flags);
//成功返回0，出错返回非0值
```

getnameinfo将一个地址转换成一个主机名和一个服务名。

套接字地址addr被翻译成一个主机名和一个服务名，若host非空，则指向一个长度为hostlen字节的缓冲区用于存放返回的主机名，若service非空，则指向一个长度为servlen字节的缓冲区用于存放返回的服务名。

flags提供控制翻译的方式。

### 将套接字与地址关联

将客户端套接字关联上一个地址可让系统选默认地址。对服务器，需给接收客户端请求的服务器套接字关联上一个众所周知的地址。客户端需发现连接服务器所需要的地址，最简单方法是服务器保留一个地址并且注册在/etc/services或者某个名字服务中。

```c
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *addr, socklen_t len);
//成功返回0 出错返回-1
```

bind用于关联地址和套接字。

对于使用的地址有以下一些限制：

1、在进程正在运行的计算机上，指定的地址必须有效；不能指定一个其他机器的地址。

2、地址必须和创建套接字时的地址族所支持的格式相匹配。

3、地址中的端口号必须不小于1024，除非该进程具有相应的特权（超级用户）。

4、一般只能将一个套接字端点绑定到一个给定地址上，尽管有些协议允许多重绑定。

因特网域，IP地址为`INADDR_ANY`则套接字端点可被绑定到所有的系统网络接口上。则可接受系统所安装任何一个网卡的数据包。

```c
#include <sys/socket.h>
int getsockname(int sockfd, struct sockaddr *restrict addr,
                socklen_t *restrict alenp);
//成功返回0，出错返回-1
```

getsockname发现绑定到套接字上的地址。alenp设置为指向整数的指针，指定缓冲区sockaddr长度，返回时，该整数被设置成返回地址的大小。地址和提供的缓冲区长度不匹配，地址会被自动截断而不报错。若没绑定，结果未定义

```c
#include <sys/socket.h>
int getpeername(int sockfd, struct sockaddr *restrict addr,
                socklen_t *restrict alenp);
```

套接字和对等方连接，可用getpeername找到对方地址。

## 建立连接

处理面向连接的网络服务，流或者报文，数据交换前，需在客户端套接字和服务器套接字间建立连接。

```c
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *addr, socklen_t len);
//成功返回0 出错返回-1
```

connect指定地址为想与之通信的服务器地址，若sockfd未绑定地址则connect会给调用者绑定一个默认地址。尝试连接可能失败，需要连接的计算机必须开启且运行，服务器必须绑定到一个想与之连接的地址，服务器等待连接的队列要有足够空间。

套接字fd处于非阻塞模式，则连接不能马上建立时，connect返回-1，errno为EINPROGRESS，应用程序可用poll或select判断文件描述符何时可写。若可写，连接完成

connect可用于无连接的网络服务DGRAM，传送的报文的目标地址会设置成connect中指定的地址，每次传送报文不需要再提供地址。仅能接收来自指定地址的报文。

```c
#include <sys/socket.h>
int listen(int sockfd, int backlog);
//成功返回0 出错返回-1
```

listen宣告服务器愿意接受连接请求，backlog提示系统该进程入队的未完成连接请求数量。实际值由系统决定。

队列满，系统会拒绝多余的连接请求，backlog值应该基于服务器期望负载和处理量选择，处理量为接受连接请求与启动服务的数量。

服务器调用listen，所用套接字能接收连接请求。

```c
#include <sys/socket.h>
int accept(int sockfd, struct sockaddr *restrict addr,
            socklen_t *restrict len);
//成功返回套接字描述符，出错返回-1
```

accept获得连接请求并建立连接。返回的fd连接到调用connect的客户端，新的套接字描述符与原始套接字描述符sockfd具有相同的套接字类型和地址族。传给accept的sockfd并没有关联到该连接而是**继续保持可用接收其他连接请求**。

若不关心客户端标识，可将addr和len设置为NULL，否则需足够大缓冲区来存放地址，len指向整数设为该缓冲区字节大小，返回时，accept填充客户端地址，更新len指向的整数。

没有连接请求在等待，accept阻塞直到一个请求到来。sockfd非阻塞，accept返回-1，errno为EAGAIN或EWOULDBLOCK。

服务器可用poll或select等待一个请求到啦，此种情况一个带有等待连接请求的套接字会以可读方式出现。

## 数据传输

6个为数据传递而设计的套接字函数中的一个，3个用于发送数据，3个用于接收数据

```c
#include <sys/socket.h>
ssize_t send(int sockfd, const void *buf, size_t nbytes, int flags);
//成功返回发送字节数，出错返回-1
```

类似write，send时**套接字必须已经连接**，nbytes和buf和write中一致，flags为send操作处理标志。

send成功返回，数据被无错误发送到网络驱动程序，而不是连接另一端。对于支持报文边界的协议，发送超过协议支持最大长度的单个报文，send出错errno为EMSGSIZE，字节流协议，send会阻塞直到整个数据传输完成。

```c
#include <sys/socket.h>
ssize_t sendto(int sockfd, const void *buf, size_t nbytes, int flags,
                const struct sockaddr *destaddr, socklen_t destlen);
//成功返回发送字节数，出错返回-1
```

sendto与send类似，可在无连接套接字上指定一个目标地址，面向连接的套接字，目标地址忽略，连接中包含目标地址，无连接的套接字除非先connect指定目标地址，否则不能send

```c
#include <sys/socket.h>
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
//成功返回发送字节数，出错返回-1
```

指定多重缓冲区传输数据，与writev相似。

```c
struct msghdr{
    void         *msg_name;            /*optional address */
    socklen_t     msg_namelen;         /*address size in bytes */
    struct iovec *msg_iov;             /*array of I/O buffers 14.6节 */
    int           msg_iovlen;          /*number of elements in array */
    void         *msg_control;         /*ancillary data */
    socklen_t     msg_controllen;      /*number of ancillary bytes */
    int           msg_flags;           /*flags for received message */
    ...
};
```

recv和read相似

```c
#include <sys/socket.h>
ssize_t recv(int sockfd, void *buf, size_t nbytes, int flags);
//返回数据字节长度；无可用数据或对等方结束，返回0，出错返回-1
```

`MSG_PEEK`标志，查看下一个读取数据，但不取走，再次调用read或recv，返回刚查看的数据。

`SOCK_STREAM`接收数据可以比预期的少，`MSG_WAITALL`标志阻止这行为直到所请求的数据全部返回，recv才返回。发送方shutdown，或网络协议支持按默认的顺序关闭并且发送端已关闭，则当所有数据接收完毕，recv返回0。

```c
#include <sys/socket.h>
ssize_t recvfrom(int sockfd, void * restrict buf, size_t len, int flags,
                struct sockaddr *restrict addr,
                socklen_t *restrict len);
//返回数据字节长度；无可用数据或对等方按序结束，返回0，出错返回-1
```

定位发送者可用recvfrom，addr非空，将包含数据发送者的套接字端点地址，addrlen指向addr缓冲区字节长度，返回时更新为实际地址字节长度。

recvfrom可用于无连接套接字。

类似于readv，将接收数据送入多个缓冲区，或接收辅助数据

```c
#include <sys/socket.h>
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
//返回数据字节长度；无可用数据或对等方按序结束，返回0，出错返回-1
```

recvmsg用msghdr结构指定接收数据的输入缓冲区，flags改变recvmsg默认行为。

服务器支持多重网络接口或多重网络协议，getaddrinfo可能返回多个候选地址，轮流尝试地址，找到一个允许连接到服务的地址便可停止。





## 套接字选项

套接字机制提供两个套接字选项接口来控制套接字行为，接口用来设置选项，另一个查询选项状态。可以获取或设置：

1、通用选项，工作在所有套接字类型上

2、套接字层次管理的选项，依赖于下层协议的支持

3、特定于某协议选项，每个协议独有

```c
#include <sys/socket.h>
int setsockopt(int sockfd, int level, int option, const void *val,
                socklen_t len);
//成功返回0 出错返回-1
```

用于设置套接字选项，level标识选项应用的协议，选项是通用的套接字层次选项则level设置成`SOL_SOCKET`，否则level设置成控制该选项的协议编号。TCP选项，level是`IPPROTO_TCP`，IP，level为`IPPROTO_IP`。

val根据选项不同指向一个数据结构或整数。整数非0启用选项，整数为0禁用选项，len指定val指向对象的大小。

```c
#include <sys/socket.h>
int getsockopt(int sockfd, int level, int option, void *restrict val,
                socklen_t *restrict lenp);
//成功返回0 出错返回-1
```

getsockopt获取选项当前值，lenp是指向整数的指针，调用前，设置为复制选项缓冲区长度，选项实际长度大则被截断，否则更新为实际长度。

## 带外数据

通信协议所支持可选功能，相比普通数据，允许更高优先级的数据传输。带外数据先行传输，即使传输队列已有数据。TCP支持，UDP不支持。

TCP带外数据被称为紧急数据，TCP仅支持一个字节的紧急数据，允许紧急数据在普通数据传递机制数据流之外传输。产生紧急数据可在3个send函数任何一个里指定`MSG_OOB`标志，若发送的字节数超过一个，最后一个字节为紧急数据字节。

紧急数据被接收，套接字若安排信号产生，则会发送SIGURG信号。

`fcntl(sockfd, F_SETOWN, pid);`

安排进程接收套接字的信号，pid正为进程ID，非-1的负值为进程组ID

`owner = fcntl(sockfd, F_GETOWN, pid);`

返回owner。TCP支持紧急标记，即在普通数据流中标记紧急数据位置，若采用套接字`SO_OOBINLINE`，可在普通数据中接收紧急数据。

```c
#include <sys/socket.h>
int sockatmark(int sockfd);
//标记处返回1，没在标记处返回0 出错返回-1
```

判断是否到达紧急标记可用sockatmark，下一个要读取的字节在标记处返回1.

带外数据出现在套接字队列，select返回一个fd且由一个待处理的异常条件。可在普通数据流上接收，也可在其中一个recv函数中采用`MSG_OOB`标志在其他队列数据前接收。

接收当前紧急数据字节前有新紧急数据到来，已有字节被丢弃。

## 非阻塞和异步I/O

recv函数没数据可用时会阻塞等待，套接字输出队列无足够空间发送消息，send函数会阻塞。套接字非阻塞模式下，函数不会阻塞而是失败，可用poll或select来判断能否接收或者传输数据。

基于套接字的异步I/O中，从套接字中读取数据，或者套接字写队列中空间变得可用时，可以安排要发送的信号SIGIO。启用异步I/O过程：

1、建立套接字所有权，这样信号可被传递到合适的进程

2、通知套接字当I/O操作不会阻塞时发送信号。

完成1的三种方式：

a、fcntl使用`F_SETOWN`

b、ioctl使用`FIOSETOWN`

c、ioctl使用`SIOCSPGRP`

完成2的两种选择：

a、fcntl使用`F_SETFL`并启用文件标志`O_ASYNC`

b、ioctl使用`FIOASYNC`
