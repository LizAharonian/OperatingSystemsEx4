/**
 * Liz Aharonian.
 * 316584960.
 */

#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <sys/types.h>
#include "osqueue.h"
#define TRUE 1
#define FALSE 0
typedef struct thread_pool
{
    pthread_t* threads;
    OSQueue* tasksQueue;
    int isStopped;
    pthread_mutex_t lock;
    void (*executeTasks)(void *);

}ThreadPool;

ThreadPool* tpCreate(int numOfThreads);

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);

#endif
