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
