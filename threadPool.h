/**
 * Liz Aharonian.
 * 316584960.
 */

#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <sys/types.h>
#include "osqueue.h"
#include<stdlib.h>
#include<unistd.h>
#include <malloc.h>
#include <pthread.h>
#include <malloc.h>
#include <pthread.h>
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#define TRUE 1
#define FALSE 0
#define ERROR "Error in system call\n"
#define STDERR 2
#define FAIL -1
#define ERROR_SIZE 21
#define SUCCESS 0

//enums
enum DestroyState {BEFORE_JOIN,AFTER_JOIN,GO,DESTROY1};
//structs
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
/**
 * tpCreate function.
 * @param numOfThreads - num of thread's pool threads.
 * @return the new thread pool.
 */
ThreadPool* tpCreate(int numOfThreads);
/**
 * tpDestroy function.
 * @param threadPool - the thread pool to be destroyed.
 * @param shouldWaitForTasks - indicate if we need to wait till all tasks would be completed.
 */
void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);
/**
 * tpInsertTask function.
 * insert new task to the queue.
 * @param threadPool - the thread pool the task is insert to.
 * @param computeFunc - the function to be performed.
 * @param param - args.
 * @return Fail or success.
 */
int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);
/**
 * execute function.
 * @param arg - args.
 * @return void *
 */
void* execute(void *arg);
/**
 * executeTasks function.
 * this function handles the thread life cycle.
 * @param arg - func args.
 */
void executeTasks(void *arg);

/**
 * handleFailure function.
 * prints error and exits the program.
 */
void handleFailure();
/**
 * join all threads function.
 * join the threads
 * @param threadPool
 */
void joinAllThreads(ThreadPool* threadPool);

#endif
