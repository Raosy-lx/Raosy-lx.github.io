# Redis学习笔记

理论和源码学习必不可少，有关redis的包括键值类型，内存数据库的特点，集群策略，持久化方式，哨兵机制等等。

### 1、redis使用

##### 1.1、redis的安装

[Linux系统安装Redis（2020最新最详细） - 净重21克 - 博客园 (cnblogs.com)](https://www.cnblogs.com/xsge/p/13841875.html#:~:text=2020%E6%9C%80%E6%96%B0Linux%E7%B3%BB%E7%BB%9F%E5%8F%91%E8%A1%8C%E7%89%88ContOS7%E6%BC%94%E7%A4%BA%E5%AE%89%E8%A3%85Redis%201%20%E3%80%81%E4%B8%8B%E8%BD%BDRedis%E5%AE%89%E8%A3%85%E7%A8%8B%E5%BA%8F%E5%8C%85....%202%20%E3%80%81%E5%AE%89%E8%A3%85.CentOS7%E5%AE%89%E8%A3%85%E6%9C%89%E9%BB%98%E8%AE%A4GCC%E7%8E%AF%E5%A2%83%EF%BC%8C%E9%BB%98%E8%AE%A44.8....%203,%E3%80%81Redis%E5%90%AF%E5%8A%A8%E6%B5%8B%E8%AF%95.Redis%206.0....%204%20%E3%80%81%E9%85%8D%E7%BD%AERedis%E4%B8%BA%E5%90%8E%E5%8F%B0%E5%90%AF%E5%8A%A8....%205%20%E3%80%81%E5%8D%B8%E8%BD%BD)

- wget

- tar -zvxf

- mv xxx /usr/local/redis

- make

- make install

- redis-server启动， redis-cli启动即可

##### 1.2、redis的使用

- set

- get
