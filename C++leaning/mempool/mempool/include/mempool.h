/*
 * =====================================================================================
 *
 *       Filename:  mempool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/16/2021 07:52:24 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  oulixiluo
 *
 * =====================================================================================
 */

#ifndef _CPPLEARNING_MEMPOOL_INCLUDE_H_
#define _CPPLEARNING_MEMPOOL_INCLUDE_H_

#include <iostream>
#include <mutex>
#include <stdlib.h>

#define BLOCK_SIZE 4096
#define CACHE_L1_LINE_SIZE (64)

#ifndef _THREAD_SAFE 
#define _THREAD_SAFE 
#endif

class CMemPool {
 public:
  static CMemPool* GetInstance();
  static void ReleaseInstance();
  int Init();
  void* allocate(unsigned size, unsigned& result_size);
  int recycle(void* mem, unsigned mem_size);
  // 伙伴算法
  void buddy_init(void *space, int block_num); //allocate buddy, block_num - 1
  void* buddy_alloc(int n); //allocate page (size of page is 2^n)
  void buddy_free(void *space, int n); //free page (starting address is space, size of page is 2^n)
  void buddy_print(); //print current state of buddy
  inline bool isValid(void* space, int n) //check if starting address (space1) is valid for length 2^n
  {
      int length = 1 << n;
      int num = ((startingBlockNum-1)%length) + 1;
      int i = ((char*)space - (char*)buddySpace) / BLOCK_SIZE; //num of first block
      if (i%length == num%length) //if starting block number is valid for length 2^n then true
        return true;
      return false;
  }

 private:
  CMemPool();
  virtual ~CMemPool();
  CMemPool(const CMemPool&) = delete;
  CMemPool& operator=(const CMemPool&) = delete;
  static CMemPool* m_MemPoolInstance;
#ifdef _THREAD_SAFE
  static std::recursive_mutex m_Mutex;
#endif
  void* buddySpace;
  int numOfEntries;
  int startingBlockNum;

  void** freeList;
};

#endif

// #ifndef _MEMORYPOOL_H
// #define _MEMORYPOOL_H
// #include <stdlib.h>
// #define MINUNITSIZE 64
// #define ADDR_ALIGN 8
// #define SIZE_ALIGN MINUNITSIZE
// #define MAXCHUNKSIZE 1024*1024*64
// struct memory_chunk;
// typedef struct memory_block
// {
// 	size_t count;
// 	size_t start;
// 	memory_chunk* pmem_chunk;
// }memory_block;
// typedef struct memory_chunk
// {
// 	size_t chunk_size;
// 	memory_block* pfree_mem_addr;
// }memory_chunk;
// typedef struct max_heap
// {
// 	memory_chunk *heap;  
// 	size_t maxSize;   
// 	size_t currentSize;  
// }max_heap;
// typedef struct MEMORYPOOL
// {
// 	void *memory;
// 	size_t size;
// 	memory_block* pmem_map; 
// 	max_heap heap;
// 	size_t mem_used_size; // 记录内存池中已经分配给用户的内存的大小
// 	size_t free_mem_chunk_count; // 记录 pfree_mem_chunk链表中的单元个数
// 	size_t mem_map_unit_count; // 
// 	size_t mem_block_count; // 一个 mem_unit 大小为 MINUNITSIZE
// }MEMORYPOOL, *PMEMORYPOOL;
// /************************************************************************/
// /* 生成内存池
// * pBuf: 给定的内存buffer起始地址
// * sBufSize: 给定的内存buffer大小
// * 返回生成的内存池指针
// /************************************************************************/
// PMEMORYPOOL CreateMemoryPool(void* pBuf, size_t sBufSize);
// /************************************************************************/
// /* 暂时没用
// /************************************************************************/
// void ReleaseMemoryPool(PMEMORYPOOL* ppMem) ; 
// /************************************************************************/
// /* 从内存池中分配指定大小的内存 
// * pMem: 内存池 指针
// * sMemorySize: 要分配的内存大小
// * 成功时返回分配的内存起始地址，失败返回NULL
// /************************************************************************/
// void* GetMemory(size_t sMemorySize, PMEMORYPOOL pMem) ;
// /************************************************************************/
// /* 从内存池中释放申请到的内存
// * pMem：内存池指针
// * ptrMemoryBlock：申请到的内存起始地址
// /************************************************************************/
// void FreeMemory(void *ptrMemoryBlock, PMEMORYPOOL pMem) ;
// #endif //_MEMORYPOOL_H