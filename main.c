#include <stdio.h>
#include "threadPool.h"

int main() {
    printf("Hello, World!\n");
    ThreadPool* threadPool = tpCreate(5);


    return 0;
}

void * function1() {
    printf("1\n");
}

void * function2() {
    printf("2\n");
}

void * function3() {
    printf("3\n");
}