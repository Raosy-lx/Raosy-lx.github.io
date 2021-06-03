# 标准I/O库

标准I/O库，由ISO C标准说明，细节包括缓冲区分配、以优化的块长度执行I/O等。

## 流和FILE对象

打开文件返回文件描述符，用于后续的I/O操作，标准I/O库围绕流进行。 当用标准I/O库打开或创建一个文件时，我们使一个流与一个文件相关联。

标准I/O文件流适用于单字节或多字节字符集，流的定向，未定向流上使用多字节I/O函数，流定向设置为宽定向，使用单字节I/O函数，则为字节定向。

```c
#include <stdio.h>
#include <wchar.h>
int fwide(FILE *fd, int mode);
//宽定向流返回正值，字节定向流返回负值，未定向流返回0
```

mode参数为负值，则试图指定流字节定向，正值，试图指定流宽定向，0不设置流定向，返回标识该流定向的值。

fwide不改变已定向流的定向，无出错返回，调用前清除errno，从fwide返回时检查errno值

打开流时，标准I/O函数fopen返回指向FILE对象的指针。

## 标准输入、标准输出和标准错误

进程预定义了三个流，引用文件与文件描述符对应，通过预定义文件指针stdin、stdout、stderr加以引用。

## 缓冲

标准I/O库提供缓冲目的在于尽可能少使用read和write调用次数，所以对I/O流自动进行缓冲管理。提供了三种类型缓冲

1、全缓冲。填满标准I/O缓冲区后才进行实际I/O操作，对于磁盘文件通常实施全缓冲，一个流上执行第一次I/O操作，通常用malloc获取所需缓冲区。

冲洗说明标准I/O缓冲区的写操作，缓冲区可通过标准I/O例程或fflush函数调用冲洗流，unix中标准I/O库冲洗对应缓冲区写入磁盘，终端驱动程序，flush对应丢弃缓冲区数据。

2、行缓冲。输入和输出中遇到换行符，标准I/O库执行I/O操作，允许一次输出一个字符，fputc，只有写了一行才实际I/O操作，流涉及一个终端时，通常用行缓冲。

行缓冲限制：缓冲区长度固定，填满缓冲区未换行也进行I/O操作；从不带缓冲流或者行缓冲流得到输入数据，会冲洗所有行缓冲流。不带缓冲流的输入需要从内核获得数据。

3、不带缓冲。标准I/O库不对字符进行缓冲存储。标准错误流stderr是不带缓冲。

ISO C要求缓冲特征：当且仅当标准输入和标准输出并不指向交互式设备时，它们才是全缓冲；标准错误决不会全缓冲。

```c
#include <stdio.h>
void setbuf(FILE *restrict fp, char *restrict buf);
int setvbuf(FILE *restrict fp, char *restrict buf, int mode, size_t size);
//成0败-1
```

给定流更换缓冲类型。流被打开后调用，在对流执行任何一个其他操作之前调用。

buf为NULL关闭缓冲，与终端设备相关，可将其设置为行缓冲。

setvbuf中的mode`_IOFBF、_IOLBF、_IONBF`对应full，line，null buf。

指定流不带缓冲，忽略buf和size，带缓冲而buf为NULL，自动分配适当长度的缓冲区，适当长度由stdio头文件中常量BUFSIZ指定。

若函数内分配一个自动变量类的标准I/O缓冲区，函数返回需要关闭流。

```c
#include <stdio.h>
int fflush(FILE *fp);
//成功0 错误 -1
```

此函数使该流所有未写数据被传送至内核，强制冲洗流。若fp为NULL，所有输出流被冲洗

## 打开流

```c
#include <stdio.h>
FILE *fopen(const char *restrict pathname, const char *restrict type);
FILE *freopen(const char *restrict pathname, const char *restrict type, FILE *restrict fp);
FILE *fdopen(int fd, const char *type);
//成功返回文件指针，出错返回NULL；
```

fopen打开指定文件，freopen在指定流上打开指定文件，该流打开则先关闭，该流定向则清除定向。一般用于将一个指定文件打开为一个预定义流：标准输入、标准输出、标准错误。

fdopen从已有fd，使标准I/O流与描述符结合，常用于创建管道和网络通信通道函数返回的描述符。

type指定对I/O流的读、写方式。指定w或a创建新文件，无法说明该文件的访问权限位。

```c
#include <stdio.h>
int fclose(FILE *fp);//成功0失败EOF
```

文件被关闭前，冲洗缓冲区的输出数据，进程正常终止，所有带未写缓冲数据标准I/O流均被冲洗，所有打开的标准I/O流都被关闭。

## 读和写流

打开流后三种不同类型非格式化I/O：每次一个字符I/O、每次一行I/O、直接I/O（二进制I/O

```c
#include <stdio.h>
int getc(FILE *fp);
int fgetc(FILE *fp);
int getchar(void);
//成功返回下一个字符，到尾或出错返回eof
```

getchar等同getc(stdin)，getc可被实现为宏，fgetc不行

getc参数可能被计算多次，fgetc是函数可将其地址作为参数传输，调用fgetc时间可能比getc长，函数调用时长通常长于宏调用。

返回下一字符将unsigned char强制转换为int。无符号最高位为1也不会返回负值，整型返回包含了所有返回情况。EOF通常为-1。

```c
#include <stdio.h>
int ferror(FILE *fp);
int feof(FILE *fp);
//条件为真返回非0，否则返回0
void clearerr(FILE *fp);
```

区分出错和文件结束。大多数实现，每个流在FILE对象中维护了这两个标志，用clearerr可以清除这两标志。

```c
#include <stdio.h>
int ungetc(int c, FILE *fp);
//成功返回c失败返回eof
```

从流中读取数据后再调用ungetc将字符压送回流。不能回送eof，ungetc会清除流文件结束标志。用ungetc目的在于需要查看下一个字符决定如何处理当前字符。压送回字符时，只是写回标志I/O库的流缓冲区中。

```c
#include <stdio.h>
int putc(int c, FILE *fp);
int fputc(int c, FILE *fp);
int putchar(int c);
```

putchar(c)等同于putc(c, stdout)，putc可被实现为宏，fputc不能。

## 每次一行I/O

```c
#include <stdio.h>
char *fgets(char *restrict buf, int n, FILE *restrict fp);
char *gets(char *buf);
//成功返回buf，到达文件尾出错返回NULL。
```

gets从标准输入读，fgets从指定流读。fgets指定缓冲区长度n。缓冲区以null为结尾，行字符超过n-1，下一次调用仍然是该行。

gets不指定缓冲区长度，可能导致缓冲区溢出，gets并不存换行符，fgets存换行符。

```c
#include <stdio.h>
int fputs(const char *restrict str, FILE *restrict fp);
int puts(const char *str);
//成功返回非负值，出错返回EOF
```

每次输出一行，fputs将一个以null字节终止的字符串写到指定的流，尾端的终止符null不写出，并不一定每次输出一行，通常null字节前为换行符。

puts不写终止符但会加入换行符，所以必输出一行。

## 标准I/O的效率

fgets和fputs比fgetc和fputc快的多，其使用memccpy实现，用汇编语言所以高效。

系统调用时长比函数调用花费更多时间。

系统不同，耗时不同。标准I/O库函数相比系统调用，耗时主要体现在用户CPU时间而非系统CPU时间

## 二进制I/O

输入数据有null和换行符 字符或行读取不能正确工作

```c
#include <stdio.h>
size_t fread(void *restrict ptr, size_t size, size_t nobj, FILE *restrict fp);
size_t fwrite(const void *restrict ptr, size_t size, size_t nobj, FILE *restrict fp);
//返回读或写的对象数。
```

读写二进制数组`fwrite(&data[2], sizeof(float), 4, fp)`

或者读写一个结构

size为结构长度，nobj为需要读写的对象数。

对于读 返回可能少于nobj，需要调用ferror和feof判断属于的情况。写少于nobj则报错。

只能用于读同一系统上已写数据。原因在于：一个结构中成员偏移量随编译程序和系统不同而不同（**数据对齐**），同一系统结构二进制存放方式可能因编译程序选项不同而不同；用于存储多字节整数和浮点值的二进制格式在不同系统结构间可能不同。

## 定位流

1、ftell 和 fseek 假定文件位置可存放在一个长整型中。

2、ftello 和 fseeko 函数。 文件偏移量可以不必一定使用长整型， off_t数据类型代替long

3、fgetpos 和 fsetpos 函数。 使用抽象数据类型 fpos_t 记录文件位置。可以根据需要定义一个足够大的数来记录文件位置。

```c
#include <stdio.h>
long ftell(FILE *fp); //成功返回当前文件位置指示 出错返回-1L
int fseek(FILE *fp, long offset, int whence); //成功返回0，出错返回-1;
void rewind(FILE *fp);
```

二进制文件，指示器在起始位置，度量单位为字节，whence与lseek中参数意义相同，即偏移量开始位置，`SEEK_SET SEEK_CUR SEEK_END`

文本文件，文件当前位置不能以简单字节偏移量来度量，非UNIX可能以不同格式存放文本文件，whence为SEEK_SET，offset为0或ftell返回值。rewind使流回到文件起始位置。

```c
#include <stdio.h>
off_t ftello(FILE *fp); //成功返回当前文件位置指示 出错返回-1L
int fseeko(FILE *fp, off_t offset, int whence); //成功返回0，出错返回-1;
```

`off_t`长于32位

```c
#include <stdio.h>
int fgetpos(FILE *restrict fp, fpos_t *restrict pos);
int fsetpos(FILE *fp, const fpos_t *pos);
//成功返回0 出错返回非0；
```

fgetpos将位置指示器的当前值存入pos所指对象，调用fsetpos，使用此值重新定位至该位置

## 格式化I/O

### 格式化输出

```c
#include <stdio.h>
int printf(const char *restrict format, ...);
int fprintf(FILE *restrict fp, const char *restrict format, ...);
int dprintf(int fd, const char *restrict format, ...);
//成功返回输出字符数，出错为负
int sprintf(char *restrict buf, const char *restrict format, ...);
//成功返回存入数组字符数，出错为负
int snprintf(char *restrict buf, size_t n, const char *restrict format, ...);
//成功返回将要存入数组字符数，出错为负
```

sprintf和snprintf 数组尾端会加null，返回值不包括null。sprintf 存在缓冲区溢出。

dprintf 不需要调用fdopen 将fd 转换为 fp， fprintf 需要。

格式说明控制其余参数的编写及显示。%-**.* * lf 宽度与字符精度在 . 左右。

```c
#include <stdarg.h>
#include <stdio.h>
int vprintf(const char *restrict format, va_list arg);
int vfprintf(FILE *restrict fp, const char *restrict format, va_list arg);
int vdprintf(int fd, const char *restrict format, va_list arg);
//成功返回输出字符数，出错为负
int vsprintf(char *restrict buf, const char *restrict format, va_list arg);
//成功返回存入数组字符数，出错为负
int vsnprintf(char *restrict buf, size_t n, const char *restrict format, va_list arg);
//成功返回将要存入数组字符数，出错为负
```

可变参数表替换成了arg

### 格式化输入

```c
#include <stdio.h>
int scanf(const char *restrict format, ...);
int fscanf(FILE *restrict fp, const char *restrict format, ...);
int sscanf(const char *restrict buf, const char *restrict format, ...);
//返回赋值的输入项数，出错或到达文件尾端，返回EOF
```

scanf用于分析输入字符串并将字符串序列转换成指定类型的变量。各参数包含了变量的地址，用转换结果对这些变量赋值。

转换说明中输入带符号的可赋予无符号类型。字段宽度和长度修饰符之间存在赋值分配符。

```c
#include <stdarg.h>
#include <stdio.h>
int vscanf(const char *restrict format, va_list arg);
int vfscanf(FILE *restrict fp, const char *restrict format, va_list arg);
int vsscanf(const char *restrict buf, const char *restrict format, va_list arg);
//返回赋值的输入项数，出错或到达文件尾端，返回EOF
```

使用有 stdarg 说明的可变长度参数表。

## 实现细节

标准I/O库都会调用第三章的I/O例程。标准I/O流都有与其相关联的fd。

```c
#include <stdio.h>
int fileno(FILE *fp);
//返回与流相关联的文件描述符
```

调用dup 或者 fcntl 需用到此函数。

## 临时文件

```c
#include <stdio.h>
char *tmpnam(char *ptr); //返回指向唯一路径名的指针 
FILE *tmpfile(void); // 成功返回文件指针，出错返回NULL
```

tmpnam产生一个与现有文件名不同的一个有效路径名字符串。每次调用都产生一个不同路径名，最多调用TMP_MAX，在stdio中定义。

若ptr为NULL， 路径名存放在一个静态区并多次调用会被重写，所以需保存路径名副本；否则返回ptr。

tmpfile创建临时二进制文件 wb+， 关闭文件或者程序结束自动删除文件。

```c
#include <stdlib.h>
char *mkdtemp(char *template); //成功返回指向目录名的指针，出错返回NULL
int mkstemp(char *template); //成功返回文件描述符，出错返回-1
```

mkdtemp函数创建目录，有唯一名字；mkstemp创建一个文件，有唯一名字。

mkstemp创建的临时文件不会自动删除，需要自己对它解除链接。

tmpnam和tempnam 返回唯一路径名和用该名字创建文件间存在时间窗口，即非原子操作。

## 内存流

标准I/O库把数据缓存在内存中，内存流的创建

```c
#include <stdio.h>
FILE *fmemopen(void *restrict buf, size_t size, const char *restrict type);
// 成功返回流指针，错误返回NULL
```

允许调用者提供缓冲区用于内存流； buf为空 fmemopen函数自动分配size字节缓冲区，关闭流，缓冲区被释放。

type控制打开方式 读 写 追加 二进制

与文件标准I/O 差别在于：

1、追加方式打开内存流，当前文件位置设置为缓冲区第一个null字节，无null则为缓冲区结尾后一个字节；非追加则为缓冲区开始位置，不适用二进制数据

2、buf为null指针，打开流读写没意义，无法找到缓冲区地址。

3、需要增加缓冲区中数据量以及调用fclose、fflush、fseek、fseeko、fsetpos都会在当前位置写入null。

```c
#include <stdio.h>
FILE *open_memstream(char **bufp, size_t *sizep);
#include <wchar.h>
FILE *open_wmemstream(wchar_t **bufp, size_t *sizep);
//成功返回流指针，出错返回NULL
```

前者面向字节，后者面向宽字节。创建流只能写打开；不能指定自己缓冲区，能访问缓冲区地址和大小；关闭流需自行释放缓冲区；对流添加字节会增加缓冲区大小。

缓冲区地址长度在调用fclose和fflush后才有效；在下一次流写入或调用fclose前有效。

缓冲区可以增长，所以可能需要重新分配，即地址可能改变。

避免缓冲区溢出，适用于创建字符串，内存流只访问主存，把标准I/O流作为参数用于临时文件的函数来说，性能提升明显。

## 标准I/O的替代软件

标准I/O库效率不高，内核和标准I/O缓冲区之间复制数据，标准I/O缓冲区和用户程序行缓冲区之间复制。

快速I/O库，使读一行函数返回指向该行指针而非复制到缓冲区。

sifo ； mmap函数； uClibc C库 
