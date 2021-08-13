#include "../include/mempool.h"

// #define _THREAD_SAFE 

int main(int argc, char* argv[]) {
    CMemPool* mymempool = CMemPool::GetInstance();
    char* a = new char;
    // std::cout << "Mempool init" << std::endl;
    // mymempool->buddy_init(a, 16);
    // mymempool->buddy_print();
    // std::cout << "Memory allocate" << std::endl;
    // char* b = (char*)mymempool->buddy_alloc(1);
    // mymempool->buddy_print();
    // std::cout << "Memory free" << std::endl;
    // mymempool->buddy_free(b, 1); // 2^1, 所需块大小均为 2的幂次方
    // mymempool->buddy_print();
    // delete a;

    // slab
    mymempool->kmem_init(a, 15);
    mymempool->kmem_cache_allInfo();
    mymempool->kmalloc(160);
    mymempool->kmem_cache_allInfo();


    return 0;
}