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

#define BLOCKSIZE 1
#define BUFFERSIZE 1000000000L
#define ITERNUM 1000000L
#define RANDNUM 1000
#define TIMES 1
#define NUMOFTHREAD 1
#define TESTTYPE 2

struct ParamForMemoryTest
{
    char *buffer1;
    char *buffer2;
    char *block;
    long bufferSize;
    long blockSize;
    long iterNum;
    int *rands;
};

void * calSeqRW(void *param)
{
    char *buffer1=((struct ParamForMemoryTest *)param)->buffer1;
    char *buffer2=((struct ParamForMemoryTest *)param)->buffer2;
    char *block=((struct ParamForMemoryTest *)param)->block;
    long blockNum=BUFFERSIZE/BLOCKSIZE;
    long loops=ITERNUM;
    for (long i=0; i<loops; i++) {
        memcpy(buffer1+BLOCKSIZE*(i%blockNum),buffer2+BLOCKSIZE*(i%blockNum),BLOCKSIZE);
    }
    pthread_exit(NULL);
}

void * calRanRW(void *param)
{
    char *buffer1=((struct ParamForMemoryTest *)param)->buffer1;
    char *buffer2=((struct ParamForMemoryTest *)param)->buffer2;
    char *block=((struct ParamForMemoryTest *)param)->block;
    int *rand=((struct ParamForMemoryTest *)param)->rands;
    long blockNum=BUFFERSIZE/BLOCKSIZE;
    long loops=ITERNUM;
    for (long i=0; i<loops; i++) {
        memcpy(buffer1+BLOCKSIZE*((i+rand[i%RANDNUM])%blockNum),buffer2+BLOCKSIZE*((i+rand[i%RANDNUM])%blockNum),BLOCKSIZE);
//        printf("%d\n",rand[i%RANDNUM]%blockNum);
    }
    pthread_exit(NULL);
}

void * calSeqLat(void *param)
{
    char *buffer1=((struct ParamForMemoryTest *)param)->buffer1;
    char *buffer2=((struct ParamForMemoryTest *)param)->buffer2;
    char *block=((struct ParamForMemoryTest *)param)->block;
    long blockNum=BUFFERSIZE;
    long loops=ITERNUM;
    struct timespec req,rem;
    req.tv_nsec=1000;
    req.tv_sec=0;
    for (long i=0; i<loops; i++) {
        memcpy(buffer1+((i*127)%blockNum),buffer2+((i*127)%blockNum),1);
        nanosleep(&req, &rem);
    }
    pthread_exit(NULL);
}

void * calRanLat(void *param)
{
    char *buffer1=((struct ParamForMemoryTest *)param)->buffer1;
    char *buffer2=((struct ParamForMemoryTest *)param)->buffer2;
    char *block=((struct ParamForMemoryTest *)param)->block;
    int *rand=((struct ParamForMemoryTest *)param)->rands;
    long blockNum=BUFFERSIZE;
    long loops=ITERNUM;
    for (long i=0; i<loops; i++) {
        memcpy(buffer1+((i+rand[i%RANDNUM])%blockNum),buffer2+((i+rand[i%RANDNUM])%blockNum),1);
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
    double t,smallestTime=1000000000;
    int testType;
    int numOfThread;
    char *buffer1;
    char *buffer2;
    char *block;
    int rands[RANDNUM*NUMOFTHREAD];
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
        memset(paramForMemoryTest[i].buffer1, 0, BUFFERSIZE);
        paramForMemoryTest[i].buffer2=(char*)malloc(BUFFERSIZE);
        memset(paramForMemoryTest[i].buffer1, 0, BUFFERSIZE);
        paramForMemoryTest[i].block=(char*)malloc(BLOCKSIZE);
        paramForMemoryTest[i].rands=rands+i*RANDNUM;
    }
    srand(time(NULL));
    for (int i=0; i<RANDNUM*NUMOFTHREAD; i++) {
        rands[i]=rand();
    }
    for (int times=0; times<TIMES; times++) {
        t=nowTimeInSec();
        for (int i=0; i<numOfThread; i++) {
            if (testType==0) {
                pthread_create(threads+i, &attr, calSeqRW, &paramForMemoryTest[i]);
            }
            else if (testType==1) {
                pthread_create(threads+i, &attr, calRanRW, &paramForMemoryTest[i]);
            }
            else if (testType==2) {
                pthread_create(threads+i, &attr, calSeqLat, &paramForMemoryTest[i]);
            }
            else if (testType==3) {
                pthread_create(threads+i, &attr, calRanLat, &paramForMemoryTest[i]);
            }
        }
        for (int i=0; i<numOfThread; i++) {
            pthread_join(threads[i], NULL);
        }
        t=nowTimeInSec()-t;
        if (smallestTime>t)
            smallestTime=t;
    }
    t=smallestTime;
    
    printf("Copy: Run %.2lfs, block size %d, with %.4lfMT/s, %.4lfGB/s, %.4lfns latency\n",t,BLOCKSIZE,ITERNUM*BLOCKSIZE*numOfThread/t/1.e6,ITERNUM*BLOCKSIZE*numOfThread/t/1.e9,t*1.e9*2/ITERNUM/numOfThread);
    
    for (int i=0; i<numOfThread; i++) {
        free(paramForMemoryTest[i].buffer1);
        free(paramForMemoryTest[i].buffer2);
        free(paramForMemoryTest[i].block);
    }
    pthread_attr_destroy(&attr);
    free(threads);
    
    return 0;
}