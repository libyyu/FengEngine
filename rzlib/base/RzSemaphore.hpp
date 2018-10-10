#ifndef __RZSEMAPHORE_HPP__
#define __RZSEMAPHORE_HPP__
#pragma once
#include <time.h>
#include "RzLock.hpp"
#include <functional>
#if PLATFORM_TARGET == PLATFORM_WINDOWS
    #include <Windows.h>
#else
    #include <semaphore.h>
    #include <sys/time.h>
    #include <limits>
#endif
_RzStdBegin
//�� -lrt �� -pthread һ������
class CRzSemaphore
{
public:
#if PLATFORM_TARGET == PLATFORM_WINDOWS
    static const uint32_t kInfinite = INFINITE;
    typedef HANDLE sem_t;
#else
    static const uint32_t kInfinite = UINT_MAX;
#endif

    CRzSemaphore(int32_t num = 0)
    {
#if PLATFORM_TARGET == PLATFORM_WINDOWS
        _sem = ::CreateSemaphore(NULL, num, LONG_MAX, NULL);
#else
        sem_init(&_sem, 0, num);
#endif    
    }
    ~CRzSemaphore()
    {
#if PLATFORM_TARGET == PLATFORM_WINDOWS
        if (NULL != _sem) 
        {
            if (0 != ::CloseHandle(_sem)) 
            {
                _sem = NULL;
            }
        }
#else
        sem_destroy(&_sem);
#endif    
    }
    
	// P
    bool wait(uint32_t millisecond = kInfinite)
    {
#if PLATFORM_TARGET == PLATFORM_WINDOWS
        if (NULL == _sem)
            return false;

        DWORD ret = ::WaitForSingleObject(_sem, millisecond);
        if (WAIT_OBJECT_0 == ret || WAIT_ABANDONED == ret) 
        {
            return true;
        } 
        else
        {
            return false;
        }
#else
        int32_t ret = 0;

        if (kInfinite == millisecond) 
        {
            ret = sem_wait(&_sem);
        } 
        else 
        {
            struct timespec ts = {0, 0};
            getAbsTimespec(&ts, millisecond);
#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
            ret = sem_timedwait(&_sem, &ts);
#else
            ret = sem_wait(&_sem);
#endif
        }

        return -1 != ret;
#endif
    }
    
	// V
    bool signal()
    {
#if PLATFORM_TARGET == PLATFORM_WINDOWS
        BOOL ret = FALSE;
        
        if (NULL != _sem) 
        {
            ret = ::ReleaseSemaphore(_sem, 1, NULL);
        }
        return TRUE == ret;
#else
        return -1 != sem_post(&_sem);
#endif    
    }
protected:
#if PLATFORM_TARGET != PLATFORM_WINDOWS
    static int32_t getAbsTimespec(struct timespec *ts, int32_t millisecond)
    {
        if (NULL == ts)
            return EINVAL;

        struct timeval tv;
        int32_t ret = gettimeofday(&tv, NULL);
        if (0 != ret)
            return ret;

        ts->tv_sec = tv.tv_sec;
        ts->tv_nsec = tv.tv_usec * 1000UL;

        ts->tv_sec += millisecond / 1000UL;
        ts->tv_nsec += millisecond % 1000UL * 1000000UL;

        ts->tv_sec += ts->tv_nsec/(1000UL * 1000UL *1000UL);
        ts->tv_nsec %= (1000UL * 1000UL *1000UL);

        return 0;
    }
#endif    

private:
    CRzSemaphore(const CRzSemaphore&){}
    void operator=(const CRzSemaphore&){}
    sem_t _sem;
};
_RzStdEnd

#endif//__RZSEMAPHORE_HPP__