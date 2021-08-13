#include "../include/mempool.h"

#define _THREAD_SAFE 

int main(int argc, char* argv[]) {
    CMemPool* mymempool = CMemPool::GetInstance();
    char* a = new char;
    mymempool->buddy_init(a, 9);
    mymempool->buddy_print();
    char* b = (char*)mymempool->buddy_alloc(1);
    mymempool->buddy_print();
    mymempool->buddy_free(b, 1);
    mymempool->buddy_print();
    // delete a;
    return 0;
}