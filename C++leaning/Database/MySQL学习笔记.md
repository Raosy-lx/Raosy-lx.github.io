# MySQL学习笔记

基础知识通过书以及课程进行学习，主要内容：数据库索引底层原理B+树，事务ACID，范式，分库分表，事务隔离级别，锁种类读写锁，MVCC多版本并发控制等等

### 1、MySQL-Centos使用

##### 1.1、安装

[CentOS安装mysql - 云+社区 - 腾讯云 (tencent.com)](https://cloud.tencent.com/developer/article/1646056)

- wget mysql链接

- tar -zvxf

- mv 包名 /usr/local/mysql

- 创建mysql用户组和用户并修改权限
  
  ```shell
  groupadd mysql
  useradd -r -g mysql mysql
  mkdir -p /data/mysql
  chown mysql:mysql -R /data/mysql
  ```

- vi /etc/my.cnf 设置配置文件
  
  ```shell
  [mysqld]
  bind-address=0.0.0.0
  port=3306
  user=mysql
  basedir=/usr/local/mysql
  datadir=/data/mysql
  socket=/tmp/mysql.sock
  log-error=/data/mysql/mysql.err
  pid-file=/data/mysql/mysql.pid
  #character config
  character_set_server=utf8mb4
  symbolic-links=0
  explicit_defaults_for_timestamp=true
  ```

- 初始化数据库（关键）
  
  目录到bin下
  
  ```shell
  ./mysqld --defaults-file=/etc/my.cnf --basedir=/usr/local/mysql --datadir=/data/mysql/ --user=mysql --initialize
  ```
  
  可能看不见临时密码，进入 /data/mysql/mysql.err 中可以看见临时密码

- 拷贝mysql.server让守护进程管控
  
  `cp /usr/local/mysql/support-files/mysql.server /etc/init.d/mysql`
  
  `service mysql start`
  
  `./mysql -uroot -p`

- 设置密码
  
  ```sql
  SET PASSWORD = PASSWORD('123456');
  ALTER USER 'root'@'localhost' PASSWORD EXPIRE NEVER;
  FLUSH PRIVILEGES;
  ```

- `service mysql stop`

##### 1.2、使用

[linux下MySQL使用方法 - D6 - 博客园 (cnblogs.com)](https://www.cnblogs.com/D666/p/9165088.html#:~:text=%E7%99%BB%E5%BD%95MySQL%E7%9A%84%E5%91%BD%E4%BB%A4%E6%98%AFmysql%EF%BC%8C%20mysql%20%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%AD%E6%B3%95%E5%A6%82%E4%B8%8B%EF%BC%9A%20mysql%20%5B-u%20username%5D%20%5B-h%20host%5D,username%20%E4%B8%8E%20password%20%E5%88%86%E5%88%AB%E6%98%AF%20MySQL%20%E7%9A%84%E7%94%A8%E6%88%B7%E5%90%8D%E4%B8%8E%E5%AF%86%E7%A0%81%EF%BC%8Cmysql%E7%9A%84%E5%88%9D%E5%A7%8B%E7%AE%A1%E7%90%86%E5%B8%90%E5%8F%B7%E6%98%AFroot%EF%BC%8C%E6%B2%A1%E6%9C%89%E5%AF%86%E7%A0%81%EF%BC%8C%E6%B3%A8%E6%84%8F%EF%BC%9A%E8%BF%99%E4%B8%AAroot%E7%94%A8%E6%88%B7%E4%B8%8D%E6%98%AFLinux%E7%9A%84%E7%B3%BB%E7%BB%9F%E7%94%A8%E6%88%B7%E3%80%82%20MySQL%E9%BB%98%E8%AE%A4%E7%94%A8%E6%88%B7%E6%98%AFroot%EF%BC%8C%E7%94%B1%E4%BA%8E%20%E5%88%9D%E5%A7%8B%E6%B2%A1%E6%9C%89%E5%AF%86%E7%A0%81%EF%BC%8C%E7%AC%AC%E4%B8%80%E6%AC%A1%E8%BF%9B%E6%97%B6%E5%8F%AA%E9%9C%80%E9%94%AE%E5%85%A5mysql%E5%8D%B3%E5%8F%AF%E3%80%82)

- 显示库 `show databases;`

- `create database <database name>;`

- `use <database name>;`

- `create table tablename (xx type auto_increment not null primary key);`

- `describe tablename;`

- 增加记录 `insert into tablename values(xxxx);`

- 修改记录 `update name set xx='xxx' where aa='bb'`

- 删除记录 `delete from name where aa='bb'`

- `drop database xxx;`

- `drop table xxx;`

- `exit`
