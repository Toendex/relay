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
    long iterNum=((struct ParamForMemoryTest *)param)->iterNum;
    PointType *nw=buffer;
    for (long i=0; i<iterNum; i++) {
//        printf("%d\n",i);
        nw=*nw;
    }
    printf("%ld\n",(long)nw);
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
    long numOfItem;
    long n;
    int step;
    int offset;
    struct ParamForMemoryTest *paramForMemoryTest;
    long iterNum;
    int numOfThread;
    long bufferSize;
    int stepInByte;
    double latency;
    double transRate;
    
    numOfThread=1;
    stepInByte=256;
    bufferSize=512000000L;
    iterNum=100000000L;
    
    if (argc>1) {
        numOfThread=atoi(argv[1]);
        if (numOfThread<=0) {
            printf("Invalid number of thread \"%d\"! Program exit.\n",numOfThread);
            exit(0);
        }
    }
    if (argc>2) {
        stepInByte=atoi(argv[2]);
        if (stepInByte<=0) {
            printf("Invalid step \"%d\"! Program exit.\n",stepInByte);
            exit(0);
        }
    }
    if (argc>3) {
        bufferSize=1000000L*atoi(argv[3]);
        if (bufferSize<=0) {
            printf("Invalid Buffer size \"%d\"! Program exit.\n",atoi(argv[3]));
            exit(0);
        }
    }
    if (argc>4) {
        iterNum=atoi(argv[4])*1000000;
        if (iterNum<=0) {
            printf("Invalid number of iteration \"%d\"! Program exit.\n",atoi(argv[4]));
            exit(0);
        }
    }

    
    numOfItem=bufferSize/sizeof(PointType *);
    step=stepInByte/sizeof(PointType *);
    threads=(pthread_t*)malloc(sizeof(pthread_t)*numOfThread);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    paramForMemoryTest=(struct ParamForMemoryTest *)malloc(sizeof(struct ParamForMemoryTest)*numOfThread);
    nodes=(PointType *)malloc(numOfItem*sizeof(PointType *));

    for (int t=0; t<numOfThread; t++) {
        n=0;
        offset=t*numOfItem/numOfThread;
        for (int i=0; i<numOfItem/numOfThread+1; i++) {
            nodes[n+offset]=(PointType)&nodes[offset+(n+step)%(numOfItem/numOfThread)];
            n=(n+step)%(numOfItem/numOfThread);
        }
    }
    for (int i=0; i<numOfThread; i++) {
        paramForMemoryTest[i].buffer=nodes+i*numOfItem/numOfThread;
        paramForMemoryTest[i].iterNum=iterNum;
    }
    

    t=nowTimeInSec();
    for (int i=0; i<numOfThread; i++) {
        pthread_create(threads+i, &attr, doIt, &paramForMemoryTest[i]);
    }
    for (int i=0; i<numOfThread; i++) {
        pthread_join(threads[i], NULL);
    }
    t=nowTimeInSec()-t;
    
    latency=t*1.e9/iterNum/numOfThread;
    transRate=iterNum/t/1.e6;
    printf("NumOfThread %d, StepInByte %d, BufferSize %ld\n",numOfThread,stepInByte,bufferSize);
    printf("Run %.2lfs,with %.4lfMT/s, %.4lfns latency\n",t,transRate,latency);
    
    
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