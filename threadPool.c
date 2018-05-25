/**
 * Liz Aharonian.
 * 316584960.
 */

#include <malloc.h>
#include <pthread.h>
#include "threadPool.h"
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include <semaphore.h>

//todo: allocations - if not null, handlefailure, free memory
//declerations
void* execute(void *arg);



ThreadPool* tpCreate(int numOfThreads) {

    ThreadPool *threadPool =(ThreadPool*) malloc(sizeof(ThreadPool));
    if (threadPool == NULL) {
        handleFailure();
    }
    threadPool->executeTasks = executeTasks;
    (*threadPool).threads =(pthread_t*) malloc(numOfThreads * sizeof(pthread_t));
    if (threadPool->threads==NULL) {
        handleFailure();
    }
    int i = 0;
    for (; i < numOfThreads; i++) {
        pthread_create((*threadPool).threads + i, NULL, execute, NULL);

    }
}
void* execute(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    (*pool).executeTasks(arg);
}
void executeTasks(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    while (!(*pool).isStopped) {
        //if queue is empty - then wait. otherwise - keep running
        sem_wait(!osIsQueueEmpty(pool->tasksQueue));
        if (!(*pool).isStopped) {
            pthread_mutex_lock(&(*pool).lockQueue);
            if (!osIsQueueEmpty((*pool).tasksQueue)) {
                //critical section - pop queue
                Task *task = (Task *) osDequeue((*pool).tasksQueue);
                pthread_mutex_unlock(&(*pool).lockQueue);
                //perform task
                task->function(task->args);
            }
        }
    }
}


void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks) {

}
/**
 * handleFailure function.
 * prints error and exits the program.
 */
void handleFailure() {
    write(STDERR, ERROR, ERROR_SIZE);
    exit(FAIL);
}

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param) {
    //create task
    Task * task = (Task*)malloc(sizeof(Task));
    if (task ==NULL) {
        handleFailure();
    }
    task->function = computeFunc;
    task->args = param;
    //add task to queue
    osEnqueue(threadPool->tasksQueue,task);

}