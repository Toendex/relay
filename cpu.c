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

#define IOPSLOOPS 1000000000L
#define FLOPSLOOPS 100000000000L
#define NUMOFTHREAD 1
#define TESTTYPE 1          //0=iops, 1=flops

int intCell(int x,long times)
{
    int i1=0x1583;
    int i2=0xa918;
    int i3=0x199d;
    int i4=65535;
    int i5=0xffff;
    int i6=0xffff;
    int i7=0xffff;
    int i8=0xffff;
    long loops=times/8;
    for (long i=0; i<loops; i++) {
        i1>>=x;
        i5<<=x;
        i2<<=x;
        i6>>=x;
        i3&=153;
        i7&=127;
        i4|=109;
        i8|=127;
    }
    int i99=i1+i2+i3+i4+i5+i6+i7+i8;
    return i99;
}

void * calIOPS(void *param)
{
    srand(time(NULL));
    int i99=intCell(rand()%10,IOPSLOOPS);
    printf("%d\n",i99);

    pthread_exit(NULL);
}

double addmulDouble(double add, double mul, long ops){
    double sum1=0.1, sum2=-0.1, sum3=0.2, sum4=-0.2, sum5=0.0, sum6=0.3, sum7=10.1, sum8=-10.1;
    double mul1=1.0, mul2= 1.1, mul3=1.2, mul4= 1.3, mul5=1.4, mul6=1.5, mul7=11.0, mul8= 11.1;
    double sum11=10.1, sum12=-10.1, sum13=10.2, sum14=-10.2, sum15=10.0, sum16=10.3, sum17=110.1, sum18=-110.1;
    double mul11=11.0, mul12= 11.1, mul13=11.2, mul14= 11.3, mul15=11.4, mul16=11.5, mul17=111.0, mul18= 111.1;
    double sum21=20.1, sum22=-20.1, sum23=20.2, sum24=-20.2, sum25=20.0, sum26=20.3, sum27=20.1, sum28=-20.1;
    long loops=ops/40;
    
    for(long i=0; i<loops; i++) {
        mul1*=mul; mul2*=mul; mul3*=mul; mul4*=mul; mul5*=mul; mul6*=mul; mul7*=mul; mul8*=mul;
        sum1+=add; sum2+=add; sum3+=add; sum4+=add; sum5+=add; sum6+=add; sum7+=add; sum8+=add;
        mul11/=mul; mul12/=mul; mul13/=mul; mul14/=mul; mul15/=mul; mul16/=mul; mul17/=mul; mul18/=mul;
        sum11-=add; sum12-=add; sum13-=add; sum14-=add; sum15-=add; sum16-=add; sum17-=add; sum18-=add;
        sum21+=add; sum22+=add; sum23+=add; sum24+=add; sum25+=add; sum26+=add; sum27+=add; sum28+=add;
    }
    return  sum1+sum2+sum3+sum4+sum5+sum6+sum7+sum8+mul1+mul2+mul3+mul4+mul5+mul6+mul7+mul8+
            sum11+sum12+sum13+sum14+sum15+sum16+sum17+sum18+mul11+mul12+mul13+mul14+mul15+mul16+mul17+mul18+
            sum21+sum22+sum23+sum24+sum25+sum26+sum27+sum28;
}

void * calFLOPS(void *param)
{
//    double d1=1.2;          //float or double?
//    double d2=2.9;
//    double d3=0.1;
//    double d4=10.7;
//    double d5=15.8;
//    double d6=10.1;
//    double d7=110.7;
//    double d8=15.8;
//
//    long loops=FLOPSLOOPS/8;
//    for (long i=0; i<loops; i++) {
//        d1+=3.1;
//        d2+=3.1;
//        d3+=3.1;
//        d4+=3.1;
//        d5+=3.1;
//        d6+=3.1;
//        d7+=3.1;
//        d8+=3.1;
//    }
//    double dd=d1+d2+d3+d4+d5+d6+d7+d8;
    double dd=addmulDouble(3.1, 1.e-8, FLOPSLOOPS);
    printf("%lf\n",dd);
    pthread_exit(NULL);
}

double timeIntervalInMS(struct timeval startTime, struct timeval endTime)
{
    return (endTime.tv_sec-startTime.tv_sec)*1000+(endTime.tv_usec-startTime.tv_usec)/1000.0;
}

int main(int argc, const char * argv[])
{
    struct timeval startTime,endTime;
    double timeInMS;
    int numOfThread;
    int testType;
    pthread_attr_t attr;
    pthread_t *threads;
    
    numOfThread=NUMOFTHREAD;
    testType=TESTTYPE;
    threads=(pthread_t*)malloc(sizeof(pthread_t)*numOfThread);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    gettimeofday(&startTime, NULL);
    for (int i=0; i<numOfThread; i++) {
        if (testType==0)
            pthread_create(threads+i, &attr, calIOPS, NULL);
        else if (testType==1)
            pthread_create(threads+i, &attr, calFLOPS, NULL);
    }
    for (int i=0; i<numOfThread; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&endTime, NULL);
    
    timeInMS=timeIntervalInMS(startTime,endTime);
    if (testType==0)
        printf("Run %.2lfms, %ld loops, with %.2lfGiops\n",timeInMS,IOPSLOOPS,IOPSLOOPS*numOfThread/timeInMS*1.e-6);
    else if (testType==1)
        printf("Run %.2lfms, %ld loops, with %.2lfGflops\n",timeInMS,FLOPSLOOPS,FLOPSLOOPS*numOfThread/timeInMS*1.e-6);
    
    
    pthread_attr_destroy(&attr);
    free(threads);
    return 0;
}