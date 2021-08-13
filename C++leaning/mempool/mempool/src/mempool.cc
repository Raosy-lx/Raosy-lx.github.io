/*
 * =====================================================================================
 *
 *       Filename:  mempool.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/16/2021 07:52:24 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  oulixiluo (luoxi), oulixiluo@tencent.com
 *        Company:  Tencent, China
 *
 * =====================================================================================
 */

#include "../include/mempool.h"
#include <ctgmath>
#include <cmath>

CMemPool* CMemPool::m_MemPoolInstance = NULL;
#ifdef _THREAD_SAFE
    std::recursive_mutex CMemPool::m_Mutex;
#endif

CMemPool::CMemPool()
{
}

CMemPool::~CMemPool()
{
    if (m_MemPoolInstance) {
        delete m_MemPoolInstance;
    }
}

CMemPool* CMemPool::GetInstance() {
#ifdef _THREAD_SAFE
    if (m_MemPoolInstance == NULL) {
        m_Mutex.lock();
        if (m_MemPoolInstance == NULL) {
            m_MemPoolInstance = new CMemPool();
        }
        m_Mutex.unlock();
    }
    return m_MemPoolInstance;
#else
    m_MemPoolInstance = new CMemPool();
    return m_MemPoolInstance;
#endif
}

void CMemPool::buddy_init(void* space, int block_num)
{
    if (space == nullptr || block_num < 2) exit(1);
#ifdef _THREAD_SAFE
    std::lock_guard<std::recursive_mutex> lock(m_Mutex);
#endif
    startingBlockNum = block_num;
    buddySpace = space;
    space = ((char*)space + BLOCK_SIZE);
    block_num--;

    int i = 1;
    numOfEntries = std::log2(block_num) + 1;

    freeList = (void**)buddySpace;
    for (i = 0; i < numOfEntries; i++)
        freeList[i] = nullptr;
    
    int maxLength = numOfEntries - 1;
    int maxLengthBlocks = 1 << maxLength;

    while (block_num > 0)
    {
        void* addr = (char*)space + (block_num - maxLengthBlocks)*BLOCK_SIZE;
        freeList[maxLength] = addr;
        *(void**)addr = nullptr;
        block_num -= maxLengthBlocks;

        if (block_num > 0)
        {
            i = 1;
            maxLength = 0;
            while (true)
            {
                if (i <= block_num && 2 * i > block_num)
                    break;
                i = i * 2;
                maxLength++;
            }
            maxLengthBlocks = 1 << maxLength;
        }
    }
}

void* CMemPool::buddy_alloc(int n)
{
    if (n < 0 || n >= numOfEntries) return nullptr;
#ifdef _THREAD_SAFE
    std::lock_guard<std::recursive_mutex> lock(m_Mutex);
#endif
    void* returningSpace = nullptr;

    if (freeList[n] != nullptr)
    {
        returningSpace = freeList[n];
        freeList[n] = *(void**)returningSpace;
        *(void**)returningSpace = nullptr;
    }
    else
    {
        for (int i = n + 1; i < numOfEntries; i++)
        {
            if (freeList[i] != nullptr)
            {
                void* ptr1 = freeList[i];
                freeList[i] = *(void**)ptr1;
                void* ptr2 = (char*)ptr1 + BLOCK_SIZE*(1 << (i-1));

                *(void**)ptr1 = ptr2;
                *(void**)ptr2 = freeList[i - 1];
                freeList[i - 1] = ptr1;

                returningSpace = buddy_alloc(n);
                break;
            }
        }
    }
    return returningSpace;
}

void CMemPool::buddy_free(void* space, int n)
{
    if (n < 0 || n >= numOfEntries) return;
#ifdef _THREAD_SAFE
    std::lock_guard<std::recursive_mutex> lock(m_Mutex);
#endif
    int bNum = 1 << n;

    if (freeList[n] == nullptr)
    {
        freeList[n] = space;
        *(void**)space = nullptr;
    }
    else
    {
        void* prev = nullptr;
        void* curr = freeList[n];
        while (curr != nullptr)
        {
            if (curr == (void*)((char*)space + BLOCK_SIZE*bNum))
            {
                if (isValid(space, n + 1))
                {
                    if (prev == nullptr)
                    {
                        freeList[n] = *(void**)freeList[n];
                    }
                    else
                    {
                        *(void**)prev = *(void**)curr;
                    }
                    buddy_free(space, n + 1);
                    return;
                }
            }
            else if (space == (void*)((char*)curr + BLOCK_SIZE*bNum))
            {
                if (isValid(curr, n + 1))
                {
                    if (prev == nullptr)
                    {
                        freeList[n] = *(void**)freeList[n];
                    }
                    else
                    {
                        *(void**)prev = *(void**)curr;
                    }

                    buddy_free(curr, n + 1);
                    return;
                }
            }

            prev = curr;
            curr = *(void**)curr;
        }

        *(void**)space = freeList[n];
        freeList[n] = space;
    }
}

void CMemPool::buddy_print()
{
    std::cout << "Buddy current state (first block,last block):" << std::endl;
    for (int i = 0; i < numOfEntries; i++)
    {
        int size = 1 << i;
        std::cout << "entry[" << i << "] (size " << size << ") -> ";
        void* curr = freeList[i];

        while (curr != nullptr)
        {
            int first = ((char*)curr - (char*)buddySpace) / BLOCK_SIZE;
            std::cout << "(" << first << "," << first + size - 1 << ") -> ";
            curr = *(void**)curr;
        }
        std::cout << "NULL" << std::endl;
    }
}