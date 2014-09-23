//
//  cpu.c
//  CS553-assignment1
//
//  Created by T on 9/11/14.
//  Copyright (c) 2014 T. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

#define IASIZE 1000
#define DASIZE 1000

//paramater struct for passing paramaters to thread
struct param
{
    long operationNum;
    int *intArray;              //integer array for IOPS
    int iaSize;                 //size of intArray
    double *doubleSArray;       //float-point array for FLOPS
//    double *doubleMArray;       //float-point array for FLOPS
    int daSize;                 //size of doubleSArray and doubleMArray
};

//call by calIOPS with run by each thread, do integer operation with "times" times
//x=integer array; arraySize=array size; times=integer operation times
int intCell(int* x,int arraySize,long times)
{
    //init
    int i99=3;
    int i88=2;
    int i77=1;
    int i66=-1;
    int i55=-2;
    int i44=-3;
    int i33=-4;
    int i22=-5;
    //outer loop times
    long loops=times/arraySize/2;
    //loop
    for (long i=0; i<loops; i++) {
        for (int j=0; j<arraySize;j+=8) {
            i99+=x[j]+x[j+1];
            i88+=x[j+1]+x[j+2];
            i77+=x[j+2]+x[j+3];
            i66+=x[j+3]+x[j+4];
            i55+=x[j+4]+x[j+5];
            i44+=x[j+5]+x[j+6];
            i33+=x[j+6]+x[j+7];
            i22+=x[j+7]+x[j];
        }
    }
    return i99+i88+i77+i66+i55+i44+i33+i22;
}

//run by each thread directly when doing integer operation. call intCell to do the real operation
void * calIOPS(void *param)
{
    struct param *p=(struct param *)param;
    int *array=p->intArray;
    int arraySize=p->iaSize;
    long operationNum=p->operationNum;
    int x=intCell(array,arraySize,operationNum);
    if (x==-1)
        printf("%d\n",x);
    pthread_exit(NULL);
}

//same as intCell() and with same structure. For float-point operation
double doubleCell(double * s,/*double * m,*/int arraySize,long times){
    double i99=3.1;
    double i88=2.2;
    double i77=1.4;
    double i66=-1.7;
    double i55=-2.1;
    double i44=-3.3;
    double i33=-4.9;
    double i22=-5.8;
    long loops=times/arraySize/2;
    for (long i=0; i<loops; i++) {
        for (int j=0; j<arraySize;j+=8) {
            //do add and multiply in the same time. It seens like that add and multiply for float-point can run in parallel
            i99+=s[j]*s[j];
            i88+=s[j+1]*s[j+1];
            i77+=s[j+2]*s[j+2];
            i66+=s[j+3]*s[j+3];
            i55+=s[j+4]*s[j+4];
            i44+=s[j+5]*s[j+5];
            i33+=s[j+6]*s[j+6];
            i22+=s[j+7]*s[j+7];
        }
    }
    return i99+i88+i77+i66+i55+i44+i33+i22;
}

//same as calIOPS, for float-point operation
void * calFLOPS(void *param)
{
    struct param *p=(struct param *)param;
    double *sArray=p->doubleSArray;
//    double *mArray=p->doubleMArray;
    int arraySize=p->daSize;
    long operationNum=p->operationNum;
    double dd=doubleCell(sArray, /*mArray,*/ arraySize, operationNum);
    printf("%lf\n",dd);
    pthread_exit(NULL);
}

//return current time in sec. with precision in microseconds
double nowTimeInSec() {
    struct timeval t;
    gettimeofday(&t,NULL);
    return (double)t.tv_sec+t.tv_usec/1000000.0;
}

int main(int argc, const char * argv[])
{
    double t;                       //timer
    int numOfThread;                //number of threads
    int testType;                   //test Type. 0=IOPS, 1=FLOPS
    pthread_attr_t attr;
    pthread_t *threads;             //threads
    struct param *p;                //a list containing parameters passing to each thread
    int iaSize=IASIZE;              //length of integer array in struct param
    int daSize=DASIZE;              //length of float-point array in struct param
    long operationNum;              //number of times of operation
  
    //Run the program in form of:
    //[Program_Name][Test_type(0=IOPS,1=FLOPS,default=0)][Number_of_thread(default=1)][Operation_number(default=1000, in million. so default=1 billion operations)]
    
    //read parameters or set default value
    operationNum=1000*1000000L;
    numOfThread=1;
    testType=0;
    if (argc>1) {
        testType=atoi(argv[1]);
        if (testType>1 || testType<0) {
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
        operationNum=1000000L*atoi(argv[3]);
        if (operationNum<=0) {
            printf("Invalid number of operation \"%d\"! Program exit.\n",atoi(argv[3]));
            exit(0);
        }
    }
    
    //init
    srand(time(NULL));
    threads=(pthread_t*)malloc(sizeof(pthread_t)*numOfThread);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    p=(struct param*)malloc(sizeof(struct param)*numOfThread);
    //different param for different threads
    for (int i=0; i<numOfThread; i++) {
        p[i].operationNum=operationNum;
        p[i].intArray=(int *)malloc(sizeof(int)*iaSize);
        p[i].doubleSArray=(double *)malloc(sizeof(double)*daSize);
//        p[i].doubleMArray=(double *)malloc(sizeof(double)*daSize);
        p[i].iaSize=iaSize;
        p[i].daSize=daSize;
        for (int j=0; j<iaSize; j++) {
            p[i].intArray[j]=rand();
        }
        for (int j=0; j<daSize; j++) {
            p[i].doubleSArray[j]=1+1.e-8*(1+rand()%5);
//            p[i].doubleMArray[j]=1+1.e-8*(1+rand()%5);
        }
    }
    
    //do operation and timing
    t=nowTimeInSec();
    for (int i=0; i<numOfThread; i++) {
        if (testType==0)
            pthread_create(threads+i, &attr, calIOPS, p+i);
        else if (testType==1)
            pthread_create(threads+i, &attr, calFLOPS, p+i);
    }
    for (int i=0; i<numOfThread; i++) {
        pthread_join(threads[i], NULL);
    }
    t=nowTimeInSec()-t;
    
    //output
    printf("Test type %d, Num Of Thread %d, Loops times %ld\n", testType, numOfThread, operationNum);
    if (testType==0)
        printf("Run %.2lfs, %ld loops, with %.2lfGiops\n",t,operationNum,operationNum*numOfThread/t/1.e9);
    else if (testType==1)
        printf("Run %.2lfs, %ld loops, with %.2lfGflops\n",t,operationNum,operationNum*numOfThread/t/1.e9);
    

    //free malloc space
    for (int i=0; i<numOfThread; i++) {
        free(p[i].intArray);
        free(p[i].doubleSArray);
//        free(p[i].doubleMArray);
    }
    free(p);
    pthread_attr_destroy(&attr);
    free(threads);
    return 0;
}