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

#define IOPSLOOPS 10000000000L
#define FLOPSLOOPS 10000000000L
#define NUMOFTHREAD 1
#define TESTTYPE 0          //0=iops, 1=flops

#define IASIZE 1000
#define DASIZE 1000

//paramater struct for passing paramaters to thread
struct param
{
    int *intArray;              //integer array for IOPS
    int iaSize;                 //size of intArray
    double *doubleSArray;       //float-point array for FLOPS
    double *doubleMArray;       //float-point array for FLOPS
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
        for (int j=0; j<arraySize;j+=4) {
//            i99+=x[j];
//            i88+=x[j+1];
//            i77+=x[j+2];
//            i66+=x[j+3];
//            i55+=x[j+4];
//            i44+=x[j+5];
//            i33+=x[j+6];
//            i22+=x[j+7];
            
            i99+=x[j]+x[j+1];
            i88+=x[j+1]+x[j+2];
            i77+=x[j+2]+x[j+3];
            i66+=x[j+3]+x[j+4];
//            i55+=x[j+4]+x[j+5];
//            i44+=x[j+5]+x[j+6];
//            i33+=x[j+6]+x[j+7];
//            i22+=x[j+7]+x[j];
        }
    }
    return i99+i88+i77+i66;
}

//run by each thread directly when doing integer operation. call intCell to do the real operation
void * calIOPS(void *param)
{
    struct param *p=(struct param *)param;
    int *array=p->intArray;
    int arraySize=p->iaSize;
    int x=intCell(array,arraySize,IOPSLOOPS);
    printf("%d\n",x);
    pthread_exit(NULL);
}

//same as intCell() and with same structure. For float-point operation
double doubleCell(double * s,double * m,int arraySize,long times){
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
            i99+=s[j]+m[j];
            i88+=s[j+1]+m[j+1];
            i77+=s[j+2]+m[j+2];
            i66+=s[j+3]+m[j+3];
            i55+=s[j+4]+m[j+4];
            i44+=s[j+5]+m[j+5];
            i33+=s[j+6]+m[j+6];
            i22+=s[j+7]+m[j+7];
        }
    }
    return i99+i88+i77+i66+i55+i44+i33+i22;
}

//same as calIOPS, for float-point operation
void * calFLOPS(void *param)
{
    struct param *p=(struct param *)param;
    double *sArray=p->doubleSArray;
    double *mArray=p->doubleMArray;
    int arraySize=p->daSize;
    double dd=doubleCell(sArray, mArray, arraySize, FLOPSLOOPS);
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
    double t;
    int numOfThread;
    int testType;
    pthread_attr_t attr;
    pthread_t *threads;
    struct param *p;
    int iaSize=IASIZE;
    int daSize=DASIZE;
    
    srand(time(NULL));
    numOfThread=NUMOFTHREAD;
    testType=TESTTYPE;
    threads=(pthread_t*)malloc(sizeof(pthread_t)*numOfThread);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    p=(struct param*)malloc(sizeof(struct param)*numOfThread);
    for (int i=0; i<numOfThread; i++) {
        p[i].intArray=(int *)malloc(sizeof(int)*iaSize);
        p[i].doubleSArray=(double *)malloc(sizeof(double)*daSize);
        p[i].doubleMArray=(double *)malloc(sizeof(double)*daSize);
        p[i].iaSize=iaSize;
        p[i].daSize=daSize;
        for (int j=0; j<iaSize; j++) {
            p[i].intArray[j]=rand();
        }
        for (int j=0; j<daSize; j++) {
            p[i].doubleSArray[j]=1+1.e-8*(1+rand()%5);
            p[i].doubleMArray[j]=1+1.e-8*(1+rand()%5);
        }
    }
    
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
    
    if (testType==0)
        printf("Run %.2lfs, %ld loops, with %.2lfGiops\n",t,IOPSLOOPS,IOPSLOOPS*numOfThread/t/1.e9);
    else if (testType==1)
        printf("Run %.2lfs, %ld loops, with %.2lfGflops\n",t,FLOPSLOOPS,FLOPSLOOPS*numOfThread/t/1.e9);
    

    //free malloc space
//    for (int i=0; i<numOfThread; i++) {
//        free(p[i].intArray);
//        free(p[i].doubleSArray);
//        free(p[i].doubleMArray);
//    }
    free(p);
    pthread_attr_destroy(&attr);
    free(threads);
    return 0;
}