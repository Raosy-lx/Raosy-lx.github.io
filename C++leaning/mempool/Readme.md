# 内存池尝试
主要是单例尝试，内存池实现算法来自 https://github.com/miloskalicanin/Slab-allocator
### 单例内存池，可选安全非安全
- 块大小为4096字节
- 采用的是伙伴算法
- 使用递归锁保证线程安全以及递归无死锁
### 使用方式
- $ make
- $ ./bin/mempool
