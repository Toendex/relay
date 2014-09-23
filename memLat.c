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

#define ITERNUM 100000000L
#define BUFFERSIZE 128000000L
#define NUMOFTHREAD 1
#define STEP 4096

typedef long PointType;

struct ParamForMemoryTest
{
    PointType *buffer;
    char *block;
    long bufferSize;
    long blockSize;
    long iterNum;
};

void * doIt(void *param)
{
    PointType *buffer=((struct ParamForMemoryTest *)param)->buffer;
    PointType *nw=buffer;
    for (long i=0; i<ITERNUM; i++) {
//        printf("%d\n",i);
        nw=*nw;
    }
    printf("%l\n",nw);
    pthread_exit(NULL);
}

double nowTimeInSec() {
    struct timeval t;
    gettimeofday(&t,NULL);
    return (double)t.tv_sec+t.tv_usec/1000000.0;
}

int main(int argc, const char * argv[])
{
    pthread_attr_t attr;
    pthread_t *threads;                             //threads
    
    double t;
    PointType * nodes,* nw;
    long numOfItem=BUFFERSIZE/sizeof(PointType *);
    long n;
    int step=STEP/sizeof(PointType *);
    int offset;
    struct ParamForMemoryTest *paramForMemoryTest;
    
    threads=(pthread_t*)malloc(sizeof(pthread_t)*NUMOFTHREAD);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    paramForMemoryTest=(struct ParamForMemoryTest *)malloc(sizeof(struct ParamForMemoryTest)*NUMOFTHREAD);
    nodes=(PointType *)malloc(numOfItem*sizeof(PointType *));

    for (int t=0; t<NUMOFTHREAD; t++) {
        n=0;
        offset=t*numOfItem/NUMOFTHREAD;
        for (int i=0; i<numOfItem/NUMOFTHREAD+1; i++) {
            nodes[n+offset]=(PointType)&nodes[offset+(n+step)%(numOfItem/NUMOFTHREAD)];
            n=(n+step)%(numOfItem/NUMOFTHREAD);
        }
    }
    for (int i=0; i<NUMOFTHREAD; i++) {
        paramForMemoryTest[i].buffer=nodes+i*numOfItem/NUMOFTHREAD;
    }
    

    t=nowTimeInSec();
    for (int i=0; i<NUMOFTHREAD; i++) {
        pthread_create(threads+i, &attr, doIt, &paramForMemoryTest[i]);
    }
    for (int i=0; i<NUMOFTHREAD; i++) {
        pthread_join(threads[i], NULL);
    }
    t=nowTimeInSec()-t;
    printf("Latency: Run %.2lfs,with %.4lfMT/s, %.4lfns latency\n",t,ITERNUM/t/1.e6,t*1.e9/ITERNUM/NUMOFTHREAD);
    
    
    //    printf("Write: block size %d, Run %.2lfms, %ld loops, with %.4lfGTps, %.4lfns latency\n",BLOCKSIZE,timeInMS,ITERNUM,ITERNUM*numOfThread/timeInMS*1.e-6,timeInMS*1.e6/(ITERNUM*numOfThread));
    
    
    //    char *block=(char *)malloc(BLOCKSIZE);
    //    int blockNum=BUFFERSIZE/BLOCKSIZE;
    //    gettimeofday(&startTime, NULL);
    //    for (long i=0; i<ITERNUM; i++) {
    //        memcpy(buffer+BLOCKSIZE*(i%blockNum),block,BLOCKSIZE);
    //    }
    //    gettimeofday(&endTime, NULL);
    //    timeInMS=timeIntervalInMS(startTime,endTime);
    //    printf("Copy: block size %d, Run %.2lfms, %ld loops, with %.4lfGBps\n",BLOCKSIZE,timeInMS,ITERNUM,ITERNUM*BLOCKSIZE/timeInMS*1.e-6);
    //    

    
    return 0;
}