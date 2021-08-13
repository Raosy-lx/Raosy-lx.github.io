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
#include <stdio.h>
#include "mempool_helper.h"

#define BLOCK_SIZE 4096
#define CACHE_L1_LINE_SIZE 64
#define CACHE_NAMELEN 20							// maximum length of cache name
#define CACHE_CACHE_ORDER 0							// cache_cache order

typedef struct kmem_cache_s kmem_cache_t;

class CMemPool {
 public:
  static CMemPool* GetInstance();
  static void ReleaseInstance();
  int Init();
  void* allocate(unsigned size, unsigned& result_size);
  int recycle(void* mem, unsigned mem_size);
  // 伙伴算法
  // 初始化页大小呈指数上升
  // 每次分配找到大于等于需求的最小块并进行划分
  // allocate buddy, block_num - 1
  void buddy_init(void *space, int block_num = 8); 
  // allocate page (size of page is 2^n)
  void* buddy_alloc(int n); 
  // free page (starting address is space, size of page is 2^n)
  void buddy_free(void *space, int n); 
  // print current state of buddy
  void buddy_print(); 
  // check if starting address (space1) is valid for length 2^n
  inline bool isValid(void* space, int n)
  {
      int length = 1 << n;
      int num = ((startingBlockNum-1)%length) + 1;
      int i = ((char*)space - (char*)buddySpace) / BLOCK_SIZE; //num of first block
      if (i%length == num%length) //if starting block number is valid for length 2^n then true
        return true;
      return false;
  }

  // slab算法
  void kmem_init(void *space, int block_num);
  // Allocate cache
  kmem_cache_t *kmem_cache_create(const char *name, size_t size, void(*ctor)(void *), void(*dtor)(void *)); 
  // Shrink cache
  int kmem_cache_shrink(kmem_cache_t *cachep); 
  // Allocate one object from cache
  void *kmem_cache_alloc(kmem_cache_t *cachep); 
  // Deallocate one object from cache
  void kmem_cache_free(kmem_cache_t *cachep, void *objp); 
  // Alloacate one small memory buffer
  void *kmalloc(size_t size); 
  // Deallocate one small memory buffer
  void kfree(const void *objp); 
  // Deallocate cache
  void kmem_cache_destroy(kmem_cache_t *cachep); 
  // Print cache info
  void kmem_cache_info(kmem_cache_t *cachep); 
  // Print error message
  int kmem_cache_error(kmem_cache_t *cachep); 
  kmem_cache_t* find_buffers_cache(const void* objp);
  void kmem_cache_allInfo();

 private:
  CMemPool();
  virtual ~CMemPool();
  CMemPool(const CMemPool&) = delete;
  CMemPool& operator=(const CMemPool&) = delete;
  static CMemPool* m_MemPoolInstance;
  static kmem_cache_t cache_cache;
  static kmem_cache_t* allCaches;
#ifdef _THREAD_SAFE
  static std::recursive_mutex m_Mutex;
  static std::mutex cout_mutex;
  static std::mutex buddy_mutex;
#endif
  void* buddySpace;
  int numOfEntries;
  int startingBlockNum;

  void** freeList;
};

#endif

