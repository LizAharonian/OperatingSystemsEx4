/**
 * Liz Aharonian.
 * 316584960.
 */


#include "threadPool.h"


ThreadPool* tpCreate(int numOfThreads) {
    //memory allocation
    ThreadPool *threadPool =(ThreadPool*) malloc(sizeof(ThreadPool));
    if (threadPool == NULL) {
        handleFailure();
    }
    threadPool->size=numOfThreads;
    threadPool->executeTasks = executeTasks;
    threadPool->tasksQueue = osCreateQueue();
    threadPool->destroyState = GO;
    (*threadPool).threads =(pthread_t*) malloc(numOfThreads * sizeof(pthread_t));
    if (threadPool->threads==NULL) {
        handleFailure();
    }
    if (pthread_mutex_init(&threadPool->lockQueue, NULL)||pthread_mutex_init(&threadPool->lockIsEmpty, NULL)
        ||pthread_mutex_init(&threadPool->lockIsStopped, NULL)) {
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
    while (pool->destroyState==GO||pool->destroyState==BEFORE_JOIN) {
        //lock mutex of queue
        pthread_mutex_lock(&(*pool).lockQueue);
        if (pool->destroyState==GO||pool->destroyState==BEFORE_JOIN) {
            if (!osIsQueueEmpty((*pool).tasksQueue)) {
                //critical section - pop queue
                Task *task = (Task *) osDequeue((*pool).tasksQueue);
                pthread_mutex_unlock(&(*pool).lockQueue);
                //perform task
                task->function(task->args);
                free(task);
            }else {
                pthread_mutex_unlock(&(*pool).lockQueue);
                sleep(1);
            }
            if(pool->destroyState==BEFORE_JOIN) {
                break;
            }
        }
    }
}


void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks) {
    //check if destroy was already called
    pthread_mutex_lock(&(*threadPool).lockIsStopped);
    if (threadPool->isStopped == TRUE) {
        return;
    }
    pthread_mutex_unlock(&(*threadPool).lockIsStopped);


    if (shouldWaitForTasks != 0) {
        //if queue is empty - then keep running. otherwise - wait
        while (1) {
            if (osIsQueueEmpty(threadPool->tasksQueue)) {
                break;
            } else {
                sleep(1);
            }
        }


        //now, the queue is empty
        joinAllThreads(threadPool);

    } else if (shouldWaitForTasks == 0) {
        //Wait for all running threads
        joinAllThreads(threadPool);
    }
}

void joinAllThreads(ThreadPool* threadPool) {
    //change state
    threadPool->destroyState=BEFORE_JOIN;
    pthread_mutex_trylock(&(*threadPool).lockIsStopped);
    threadPool->isStopped = TRUE;
    pthread_mutex_unlock(&(*threadPool).lockIsStopped);

    int i=0;
    for (;i<threadPool->size;i++) {
        pthread_join(threadPool->threads[i],NULL);
    }
    while (!osIsQueueEmpty(threadPool->tasksQueue)) {
        Task* task = osDequeue(threadPool->tasksQueue);
        free(task);
    }
    threadPool->destroyState=AFTER_JOIN;
    //free of memory allocation
    free(threadPool->threads);
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

    return SUCCESS;

}