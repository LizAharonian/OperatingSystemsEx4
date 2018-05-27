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
void executeTasks(void *arg);



ThreadPool* tpCreate(int numOfThreads) {

    ThreadPool *threadPool =(ThreadPool*) malloc(sizeof(ThreadPool));
    if (threadPool == NULL) {
        handleFailure();
    }
    threadPool->size=numOfThreads;
    threadPool->executeTasks = executeTasks;
    threadPool->tasksQueue = osCreateQueue();
    threadPool->destroyState = GO;
    if (pthread_mutex_init(&threadPool->lockQueue, NULL)||pthread_mutex_init(&threadPool->lockIsEmpty, NULL)
        ||pthread_mutex_init(&threadPool->lockIsStopped, NULL)) {
        handleFailure();
    }


    (*threadPool).threads =(pthread_t*) malloc(numOfThreads * sizeof(pthread_t));
    //lock is empty
    pthread_mutex_lock(&threadPool->lockIsEmpty);
    if (threadPool->threads==NULL) {
        handleFailure();
    }
    int i = 0;
    for (; i < numOfThreads; i++) {
        pthread_create((*threadPool).threads + i, NULL, execute, threadPool);

    }
    return threadPool;
}
void* execute(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    (*pool).executeTasks(arg);
}
void executeTasks(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    while (pool->destroyState==GO||pool->destroyState==BEFORE_JOIN||pool->destroyState==DESTROY1) {
        //if queue is empty - then wait. otherwise - keep running
        // sem_wait(!osIsQueueEmpty(pool->tasksQueue));
        //sem_wait();
        pthread_mutex_lock(&(*pool).lockQueue);
        if (pool->destroyState==GO||pool->destroyState==BEFORE_JOIN||pool->destroyState==DESTROY1) {
            if (osIsQueueEmpty(pool->tasksQueue)) {
                if(pool->destroyState==DESTROY1){
                    break;
                }
                pthread_mutex_lock(&pool->lockIsEmpty);

            }
            if (!osIsQueueEmpty((*pool).tasksQueue)) {
                //critical section - pop queue
                Task *task = (Task *) osDequeue((*pool).tasksQueue);
                pthread_mutex_unlock(&(*pool).lockQueue);
                //perform task
                task->function(task->args);
                free(task);


            }else {
                pthread_mutex_unlock(&(*pool).lockQueue);
                //sleep(1);

            }
            if(pool->destroyState==BEFORE_JOIN) {
                break;
            }
        }
    }
}


void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks) {
    pthread_mutex_lock(&(*threadPool).lockIsStopped);
    if (threadPool->isStopped == TRUE) {
        return;
    }
    pthread_mutex_unlock(&(*threadPool).lockIsStopped);


    if (shouldWaitForTasks != 0) {
        threadPool->destroyState=DESTROY1;
        //if queue is empty - then keep running. otherwise - wait
       /* while (1) {
            if (osIsQueueEmpty(threadPool->tasksQueue)) {
                break;
            } else {
                sleep(1);
            }
        }*/


        //now, the queue is empty
        joinAllThreads(threadPool);

    } else if (shouldWaitForTasks == 0) {
        //Wait for all running threads
        threadPool->destroyState=BEFORE_JOIN;
        joinAllThreads(threadPool);
    }

    //pthread_mutex_unlock(&(*threadPool).lockQueue);


}

void joinAllThreads(ThreadPool* threadPool) {

    pthread_mutex_trylock(&(*threadPool).lockIsStopped);
    threadPool->isStopped = TRUE;
    pthread_mutex_unlock(&(*threadPool).lockIsStopped);

    int i=0;
    for (;i<threadPool->size;i++) {
        pthread_join(threadPool->threads[i],NULL);
    }
    threadPool->destroyState=AFTER_JOIN;

    //indicats the thread pool is empty
   // threadPool->tasksQueue->head==NULL;
    //threadPool->tasksQueue->tail==NULL;

   /* if (pthread_mutex_destroy(&threadPool->lockQueue)||pthread_mutex_destroy(&threadPool->lockIsEmpty)
        ||pthread_mutex_destroy(&threadPool->lockIsStopped)) {
        handleFailure();
    }*/
    for (;i<threadPool->size;i++) {
        free(threadPool->threads[i]);
    }
    osDestroyQueue(threadPool->tasksQueue);
    free(threadPool);
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
    int isWasEmpty=FALSE;
    if (osIsQueueEmpty(threadPool->tasksQueue)) {
        isWasEmpty = TRUE;
    }
    //In case on destroy was performed
    if ((*threadPool).isStopped) {
        return FAIL;
    }
    //create task
    Task * task = (Task*)malloc(sizeof(Task));
    if (task ==NULL) {
        handleFailure();
    }
    task->function = computeFunc;
    task->args = param;

    //add task to queue
    pthread_mutex_lock(&threadPool->lockQueue);

    osEnqueue(threadPool->tasksQueue,task);
    pthread_mutex_unlock(&threadPool->lockQueue);

    if(isWasEmpty) {
        pthread_mutex_unlock(&threadPool->lockIsEmpty);
    }

    return SUCCESS;

}