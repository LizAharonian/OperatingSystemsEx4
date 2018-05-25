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
//declerations
void* execute(void *arg);



ThreadPool* tpCreate(int numOfThreads) {

    ThreadPool *threadPool = malloc(sizeof(ThreadPool));
    (*threadPool).threads = malloc(numOfThreads * sizeof(pthread_t));
    int i = 0;
    for (; i < numOfThreads; i++) {
       // pthread_create((*threadPool).threads + i, NULL, execute, NULL);

    }
}
void* execute(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    (*pool).executeTasks(arg);
}
void executeTasks(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    while (!(*pool).isStopped) {
        pthread_mutex_lock(&(*pool).lock);
        if (!osIsQueueEmpty((*pool).tasksQueue)) {
           /* Task* task = tasksQueue.front();
            tasksQueue.pop();
            pthread_mutex_unlock(&lock);
            task->execute();*/
        }
        else {
           // pthread_mutex_unlock(&lock);
            //sleep(1);
        }
    }
}


void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks) {

}

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param) {

}