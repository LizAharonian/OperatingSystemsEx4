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
#define ERROR "Error in system call\n"
#define STDERR 2
#define FAIL -1
#define ERROR_SIZE 21
#define SUCCESS 0

enum DestroyState {BEFORE_JOIN,AFTER_JOIN,GO,DESTROY1};
//typedef enum destroyState {FIRST, SECOND} DestroyState;

typedef struct thread_pool
{
    int size;
    pthread_t* threads;
    OSQueue* tasksQueue;
    int isStopped;
    pthread_mutex_t lockQueue;
    pthread_mutex_t lockIsStopped;
    pthread_mutex_t lockIsEmpty;
    void (*executeTasks)(void *);
    enum DestroyState destroyState;

}ThreadPool;

typedef struct task
{
    void (*function)(void *);
    void * args;

}Task;



ThreadPool* tpCreate(int numOfThreads);

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);

#endif
