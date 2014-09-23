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

#define RANDNUM 1000
#define TIMES 1

struct ParamForMemoryTest
{
    char *buffer1;              //Source buffer for memcpy
    char *buffer2;              //Destination buffer for memcpy
    long bufferSize;            //Buffer size for both buffer1 and buffer2
    long blockSize;             //Block size for each memcpy() call
    int iterNum;                //Iteration number. Total amount of data that memcpy operates is iterNum*bufferSize*2 (2 for source and destination)
    int randSeed;               //Seed for srand() function for random memcpy
//    int *rands;
};

//sequential memory copy(read+write)
void * calSeqRW(void *param)
{
    char *buffer1=((struct ParamForMemoryTest *)param)->buffer1;
    char *buffer2=((struct ParamForMemoryTest *)param)->buffer2;
    long bufferSize=((struct ParamForMemoryTest *)param)->bufferSize;
    long blockSize=((struct ParamForMemoryTest *)param)->blockSize;
    int iterNum=((struct ParamForMemoryTest *)param)->iterNum;
    long blockNum=bufferSize/blockSize;
    long loops=iterNum*blockNum;                //number of loops
    for (long i=0; i<loops; i++) {
        memcpy(buffer1+blockSize*(i%blockNum),buffer2+blockSize*(i%blockNum),blockSize);
    }
    pthread_exit(NULL);
}

//random memory copy(read+write)
void * calRanRW(void *param)
{
    char *buffer1=((struct ParamForMemoryTest *)param)->buffer1;
    char *buffer2=((struct ParamForMemoryTest *)param)->buffer2;
//    int *rand=((struct ParamForMemoryTest *)param)->rands;
    long bufferSize=((struct ParamForMemoryTest *)param)->bufferSize;
    long blockSize=((struct ParamForMemoryTest *)param)->blockSize;
    int iterNum=((struct ParamForMemoryTest *)param)->iterNum;
    long blockNum=bufferSize/blockSize;
    long loops=iterNum*blockNum;                //number of loops
    
    int ranSeed=((struct ParamForMemoryTest *)param)->randSeed;
    srand(ranSeed);
    
    for (long i=0; i<loops; i++) {
//        memcpy(buffer1+BLOCKSIZE*((i+rand[i%RANDNUM])%blockNum),buffer2+BLOCKSIZE*((i+rand[i%RANDNUM])%blockNum),BLOCKSIZE);
        memcpy(buffer1+blockSize*((i+rand())%blockNum),buffer2+blockSize*((i+rand())%blockNum),blockSize);
    }
    pthread_exit(NULL);
}

//sequential memory copy(read+write) with block size=1
void * calSeqLat(void *param)
{
    char *buffer1=((struct ParamForMemoryTest *)param)->buffer1;
    char *buffer2=((struct ParamForMemoryTest *)param)->buffer2;
    long bufferSize=((struct ParamForMemoryTest *)param)->bufferSize;
    long blockSize=((struct ParamForMemoryTest *)param)->blockSize;
    int iterNum=((struct ParamForMemoryTest *)param)->iterNum;
    long blockNum=bufferSize/blockSize;
    long loops=iterNum*blockNum;
//    struct timespec req,rem;
//    req.tv_nsec=1000;
//    req.tv_sec=0;
    for (long i=0; i<loops; i++) {
        memcpy(buffer1+((i*129)%blockNum),buffer2+((i*129)%blockNum),1);  //(i*129) to deal with cache line size
//        nanosleep(&req, &rem);
    }
    pthread_exit(NULL);
}

//random memory copy(read+write) with block size=1
void * calRanLat(void *param)
{
    char *buffer1=((struct ParamForMemoryTest *)param)->buffer1;
    char *buffer2=((struct ParamForMemoryTest *)param)->buffer2;
//    int *rand=((struct ParamForMemoryTest *)param)->rands;
    long bufferSize=((struct ParamForMemoryTest *)param)->bufferSize;
    long blockSize=((struct ParamForMemoryTest *)param)->blockSize;
    int iterNum=((struct ParamForMemoryTest *)param)->iterNum;
    long blockNum=bufferSize/blockSize;
    long loops=iterNum*blockNum;
    
    int ranSeed=((struct ParamForMemoryTest *)param)->randSeed;
    srand(ranSeed);
    
    for (long i=0; i<loops; i++) {
//        memcpy(buffer1+((i+rand[i%RANDNUM])%blockNum),buffer2+((i+rand[i%RANDNUM])%blockNum),1);
        memcpy(buffer1+((i+rand())%blockNum),buffer2+((i+rand())%blockNum),1);
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
    double t,smallestTime=1000000000;               //timer
    int testType;                                   //test type.
                                                        //0 = seq memcpy; 1 = ran memcpy;
                                                        //2 = seq memory latency; 3 = ran memory latency
    int numOfThread;                                //number of threads
    long blockSize;                                 //block size for each call of memcpy.
                                                        //Doesn't affect memory latency test(which always use block size = 1)
    long bufferSize;                                //buffer size
    int iterNum;
    char *buffer1;                                  //first buffer. The source of memcpy
    char *buffer2;                                  //second buffer. The destination of memcpy
//    int rands[RANDNUM*NUMOFTHREAD];                 //array contains random numbers. for random test
    pthread_attr_t attr;
    pthread_t *threads;                             //threads
    struct ParamForMemoryTest *paramForMemoryTest;  //parameter struct passed to threads
    double latency;
    double bandwidth;
    double transRate;
    
    //Run the program in form of:
    //[Program_Name][Test_type(default=0)][Number_of_thread(default=1)][Iterate_num(default=1. Every iterate will copy 1GB memory space to 1GB memory space)][Buffer_Size_In_MB(default=1000, so, 1GB source and 1GB destination)] [Block_size_without_unit(default size=1k)][Unit_of_block_size(b,kb,mb,gb,default=byte)]
    
    //read parameters or set default value
    testType=0;
    numOfThread=1;
    iterNum=1;
    bufferSize=1000*1000000L;
    blockSize=1000L;
    if (argc>1) {
        testType=atoi(argv[1]);
        if (testType>3 || testType<0) {
            printf("Invalid test type \"%d\"! Program exit.\n",testType);
            exit(0);
        }
    }
    if (argc>2) {
        numOfThread=atoi(argv[2]);
        if (numOfThread<=0) {
            printf("Invalid number of thread \"%d\"! Program exit.\n",numOfThread);
            exit(0);
        }
    }
    if (argc>3) {
        iterNum=atoi(argv[3]);
        if (iterNum<=0) {
            printf("Invalid number of iteration \"%d\"! Program exit.\n",iterNum);
            exit(0);
        }
    }
    if (argc>4) {
        bufferSize=1000000L*atoi(argv[4]);
        if (bufferSize<=0) {
            printf("Invalid Buffer size \"%d\"! Program exit.\n",atoi(argv[4]));
            exit(0);
        }
    }
    if (argc>5) {
        blockSize=atoi(argv[5]);
        if (blockSize<=0) {
            printf("Invalid Block size \"%d\"! Program exit.\n",atoi(argv[5]));
            exit(0);
        }
    }
    if (argc>6) {
        if(argv[6][0]=='k' || argv[6][0]=='K' )
            blockSize*=1000L;
        else if(argv[6][0]=='m' || argv[6][0]=='M' )
            blockSize*=1000000L;
        else if(argv[6][0]=='g' || argv[6][0]=='G' )
            blockSize*=1000000000L;
    }
    if (testType>1) {
        blockSize=1;
    }
    
    //init
    buffer1=(char*)malloc(bufferSize);
    buffer2=(char*)malloc(bufferSize);
    threads=(pthread_t*)malloc(sizeof(pthread_t)*numOfThread);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    //init the buffer content
    memset(buffer1,1,bufferSize);
    memset(buffer2,2,bufferSize);
    //init parameters passing to thread
    paramForMemoryTest=(struct ParamForMemoryTest *)malloc(sizeof(struct ParamForMemoryTest)*numOfThread);
    srand(time(NULL));
//    for (int i=0; i<RANDNUM*numOfThread; i++) {
//        rands[i]=rand();
//    }
    for (int i=0; i<numOfThread; i++) {
        paramForMemoryTest[i].buffer1=buffer1+i*bufferSize/numOfThread;
        paramForMemoryTest[i].buffer2=buffer2+i*bufferSize/numOfThread;
//        paramForMemoryTest[i].rands=rands+i*RANDNUM;
        paramForMemoryTest[i].bufferSize=bufferSize/numOfThread;
        paramForMemoryTest[i].blockSize=blockSize;
        paramForMemoryTest[i].iterNum=iterNum;
        paramForMemoryTest[i].randSeed=rand();
    }
    
    //do operation and timing
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
    
    //output result
    transRate=2*iterNum*bufferSize/blockSize/t/1.e6;
    bandwidth=2*iterNum*bufferSize/t/1.e9;
    latency=t*1.e9/iterNum/bufferSize*blockSize/2;
    printf("Block size %ld, Buffer size %ld, Test type %d, Num Of Thread %d, Iter %d\n", blockSize, bufferSize, testType, numOfThread, iterNum);
    printf("Run %.2lfs, with %.4lfMT/s, %.4lfGB/s, %.4lfns latency\n",t,transRate,bandwidth,latency);
    
    //free
    
//    for (int i=0; i<numOfThread; i++) {
//        free(paramForMemoryTest[i].buffer1);
//        free(paramForMemoryTest[i].buffer2);
//    }
    free(buffer1);
    free(buffer2);
    pthread_attr_destroy(&attr);
    free(threads);
    
    return 0;
}