# Makefile学习与使用

makefile可以用于管理工程项目，整合多个源文件

参考：

[多个文件目录下Makefile的写法 - feng..liu - 博客园](https://www.cnblogs.com/fengliu-/p/10218450.html)

https://zhuanlan.zhihu.com/p/341398357

[学习总结:工程管理与makefile - 程序园](http://www.voidcn.com/article/p-dbcwikol-bsb.html)

## 创建工程项目

推荐一个工程包含4个文件夹：

`include`: 用于存放头文件（.h）

`bin`: 用于存放二进制可执行文件（make最终创建的文件）

`obj`: 存放编译过程产生的目标文件（.o）

`src`: 存放源文件（.c 或 .cpp）

工程编写完成，通过makefile的make生成可执行文件，即可执行

## 创建makefile文件

```c
[user@linux ~]$ vim makefile
/* vim操作 输入makefile中的工程内容 */
```

linux下创建makefile文件，推荐vim编写，不易出现格式错误

### makefile编写规范

编写规范中的规则参照shell脚本规则，如$符号的使用

```shell
INCLUDE =./include
SRC = ./src
OBJ = ./obj
BIN = ./bin
# 4个文件目录

SOURCES = $(wildcard $(SRC)/*.c)
# wildcard 函数 产生一个所有以.c .cpp结尾的文件列表，存入SOURCES
# 得到SRC目录下的源文件列表，包含目录项

OBJS = $(patsubst %.c,$(OBJ)/%.o, $(notdir $(SOURCES)))
# 创建目标文件列表
# notdir去除源文件列表的目录信息，SRC里的文件名列表将只有文件名
# 例：./src/a.c → a.c → ./obj/a.o
# (patsubst %.src , %.dst , $(dir))
# 把 dir中变量 src后缀的替换为 dst后缀 %匹配0或若干字符

TARGET = $(BIN)/main
# 创建可执行文件 main

XX = g++
CC = c++
# 编译器信息

CFLAGS = -Wall -g -O -I$(INCLUDE)
# 配置编译器设置 
# -Wall输出警告信息 
# -O编译时优化 
# -g编译debug版本
# -I$(INCLUDE): $(INCLUDE)作为第一个寻找头文件的目录

LIBS = -lm
# 使用的函数库，在默认Lib中找寻lm.so动态库
# 如果gcc编译选项加入-static则是lm.a静态库
# -L + 目录 可以指定第一个找寻库文件目录

$(TARGET): $(OBJS)
	$(CC) -o $@  $^
# 用所有的目标文件生成目的可执行文件
# 内部变量 $@ 为当前规则的目的文件名 此处等价于 ${TARGET}
# $^ 表示所有的依赖文件
# $<  表示第一个依赖文件
# $?  表示比目标还要新的依赖文件列表
# 依赖文件：是生成目标依赖的文件或目标

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<
# 通过源文件生成各个目标文件

.PHONY: clean
# clean是伪文件，make过程不会生成

clean:
	rm -f $(TARGET) $(OBJS)
# 清除生成的目标文件和可执行文件

```


