/**
 * Liz Aharonian.
 * 316584960.
 */


#include <fcntl.h>
#include "threadPool.h"


ThreadPool* tpCreate(int numOfThreads) {
    int programOutputFD;
    if ((programOutputFD = open("programOutput.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0) {
        handleFailure();
    }
    if (dup2(programOutputFD, 1) == FAIL) {
        handleFailure();
    }
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
    //init mutex
    if (pthread_mutex_init(&threadPool->lockQueue, NULL)||pthread_mutex_init(&threadPool->lockIsEmpty, NULL)
        ||pthread_mutex_init(&threadPool->lockIsStopped, NULL)||(pthread_cond_init(&(threadPool->notify), NULL))) {
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
        //handle busy waiting
        printf("busy\n");
        if (osIsQueueEmpty(pool->tasksQueue) && (pool->destroyState==GO||pool->destroyState==BEFORE_JOIN)) {
            printf("before wait\n");
            //lock mutex of queue
            pthread_mutex_lock(&(*pool).lockQueue);
            //we want to wait until there are tasks in queue
            pthread_cond_wait(&(pool->notify), &(pool->lockQueue));
        }else if(pool->destroyState==DESTROY1&&osIsQueueEmpty(pool->tasksQueue)) {
            printf("enter end destroy\n");
            break;
        }else {
            pthread_mutex_lock(&(*pool).lockQueue);
        }
        if (pool->destroyState==GO||pool->destroyState==BEFORE_JOIN||pool->destroyState==DESTROY1) {
            if (!osIsQueueEmpty((*pool).tasksQueue)) {
                //critical section - pop queue
                Task *task = (Task *) osDequeue((*pool).tasksQueue);
                pthread_mutex_unlock(&(*pool).lockQueue);
                //perform task
                task->function(task->args);
                free(task);
            }else {
                pthread_mutex_unlock(&(*pool).lockQueue);
            }
            if(pool->destroyState==BEFORE_JOIN) {
                break;
            }
        }else {
            pthread_mutex_unlock(&(*pool).lockQueue);
        }
    }
    printf("exit while");
}


void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks) {


    //check if destroy was already called
    pthread_mutex_lock(&(*threadPool).lockIsStopped);
    if (threadPool->isStopped == TRUE) {
        return;
    }
    pthread_mutex_unlock(&(*threadPool).lockIsStopped);
    //lock queue
    pthread_mutex_lock(&(*threadPool).lockQueue);
    if((pthread_cond_broadcast(&(threadPool->notify)) != 0) ||
       (pthread_mutex_unlock(&(threadPool->lockQueue)) != 0)) {
        handleFailure();
    }

    if (shouldWaitForTasks != 0) {
        //now, the queue is empty
        threadPool->destroyState=DESTROY1;
        joinAllThreads(threadPool);

    } else if (shouldWaitForTasks == 0) {
        //Wait for all running threads
        threadPool->destroyState=BEFORE_JOIN;
        joinAllThreads(threadPool);
    }
}

void joinAllThreads(ThreadPool* threadPool) {
    //change state
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

    if(pthread_cond_signal(&(threadPool->notify)) != 0) {
        handleFailure();
    }

    pthread_mutex_unlock(&threadPool->lockQueue);


    return SUCCESS;

}