#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include "threadPool.h"

void * function1();
void * function2();
void * function3();



//המספרים צריכים להיות מודפסים בצורה סנכרונית, קודם 11..1 ואז 2...2 ואז 3...3
/*int main() {
    ThreadPool* threadPool =tpCreate(1);
    char * args = (char *)malloc(10);
    tpInsertTask(threadPool,function1,args);
    tpInsertTask(threadPool,function2,args);
    tpInsertTask(threadPool,function3,args);
    int temp;
    scanf("%d",&temp);
    return 0;
}*/

/*//מספרים מודפסים באיזה סדר שבא להם
int main() {
    ThreadPool* threadPool =tpCreate(3);
    char * args = (char *)malloc(10);
    tpInsertTask(threadPool,function1,args);
    tpInsertTask(threadPool,function2,args);
    tpInsertTask(threadPool,function3,args);
    int temp;
    scanf("%d",&temp);
    return 0;
}*/


int main() {
    ThreadPool *threadPool = tpCreate(3);
    char *args = (char *) malloc(10);

    tpInsertTask(threadPool, function1, args);
    tpInsertTask(threadPool, function2, args);
    tpInsertTask(threadPool, function3, args);

    tpDestroy(threadPool, 0);
    int temp;
    scanf("%d", &temp);
    return 0;
}

void * function1() {
    int i;
    for(i=1; i<100;i++) {
        printf("1\n");

    }
}
void * function1WithSleep() {
    int i;
    for(i=1; i<100;i++) {
        printf("1\n");
        sleep(1);

    }
}

void * function2() {
    int i;
    for(i=1; i<100;i++) {
        printf("2\n");
    }
}
void * function2Sleep() {
    int i;
    for(i=1; i<100;i++) {
        printf("2\n");
        sleep(1);
    }
}

void * function3() {
    int i;
    for(i=1; i<100;i++) {
        printf("3\n");

    }
}
void * function3Sleep() {
    int i;
    for(i=1; i<100;i++) {
        printf("3\n");
        sleep(1);

    }
}