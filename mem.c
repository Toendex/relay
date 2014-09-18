//
//  mem.c
//  CS553-assignment1
//
//  Created by T on 9/11/14.
//  Copyright (c) 2014 T. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>

#define BLOCKSIZE 10000
#define BUFFERSIZE 100000000
#define ITERNUM 1000000L
#define NUMOFTHREAD 4
#define TESTTYPE 0          

struct ParamForMemoryTest
{
    char *buffer1;
    char *buffer2;
    char *block;
    long bufferSize;
    long blockSize;
    long iterNum;
};

void * calSeqRW(void *param)
{
    char *buffer1=((struct ParamForMemoryTest *)param)->buffer1;
    char *buffer2=((struct ParamForMemoryTest *)param)->buffer2;
    char *block=((struct ParamForMemoryTest *)param)->block;
    int blockNum=BUFFERSIZE/BLOCKSIZE;
    long loops=ITERNUM;
    for (long i=0; i<loops; i+=2) {
        memcpy(buffer1+BLOCKSIZE*(i%blockNum),buffer2+BLOCKSIZE*(i%blockNum),BLOCKSIZE);
        memcpy(buffer1+BLOCKSIZE*((i+1)%blockNum),buffer2+BLOCKSIZE*((i+1)%blockNum),BLOCKSIZE);
    }
    pthread_exit(NULL);
}
void * calRanRW(void *param)
{
    srand(time(NULL));
    char *buffer1=((struct ParamForMemoryTest *)param)->buffer1;
    char *block=((struct ParamForMemoryTest *)param)->block;
    int blockNum=BUFFERSIZE/BLOCKSIZE;
    long loops=ITERNUM;
    for (long i=0; i<loops; i++) {
        memcpy(buffer1+BLOCKSIZE*(rand()%blockNum),block,BLOCKSIZE);
    }
    pthread_exit(NULL);
}

double nowTimeInSec() {
    struct timeval t;
    gettimeofday(&t,NULL);
    return (double)t.tv_sec+t.tv_usec/1000000.0;
}

int main(int argc, const char * argv[])
{
    double t;
    int testType;
    int numOfThread;
    char *buffer1;
    char *buffer2;
    char *block;
    pthread_attr_t attr;
    pthread_t *threads;
    
    testType=TESTTYPE;
    numOfThread=NUMOFTHREAD;
    threads=(pthread_t*)malloc(sizeof(pthread_t)*numOfThread);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    struct ParamForMemoryTest *paramForMemoryTest;
    paramForMemoryTest=(struct ParamForMemoryTest *)malloc(sizeof(struct ParamForMemoryTest)*numOfThread);
    for (int i=0; i<numOfThread; i++) {
        paramForMemoryTest[i].buffer1=(char*)malloc(BUFFERSIZE);
        paramForMemoryTest[i].buffer2=(char*)malloc(BUFFERSIZE);
        paramForMemoryTest[i].block=(char*)malloc(BLOCKSIZE);
    }
    
    t=nowTimeInSec();
    for (int i=0; i<numOfThread; i++) {
        if (testType==0) {
            pthread_create(threads+i, &attr, calSeqRW, &paramForMemoryTest[i]);
        }
    }
    for (int i=0; i<numOfThread; i++) {
        pthread_join(threads[i], NULL);
    }
    t=nowTimeInSec()-t;
    
    printf("Copy: Run %.2lfs, block size %d, %ld loops, with %.4lfGTps, %.4lfGBps, %.4lfns latency\n",t,BLOCKSIZE,ITERNUM*numOfThread,ITERNUM*numOfThread/t/1.e9,ITERNUM*BLOCKSIZE*numOfThread/t/1.e9,t/ITERNUM/numOfThread);
    
    for (int i=0; i<numOfThread; i++) {
        free(paramForMemoryTest[i].buffer1);
        free(paramForMemoryTest[i].buffer2);
        free(paramForMemoryTest[i].block);
    }
    pthread_attr_destroy(&attr);
    free(threads);
    
    return 0;
}