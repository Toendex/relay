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
#define BUFFERSIZE 512000000L
#define STEP 1024

typedef long PointType;

double nowTimeInSec() {
    struct timeval t;
    gettimeofday(&t,NULL);
    return (double)t.tv_sec+t.tv_usec/1000000.0;
}

int main(int argc, const char * argv[])
{
    double t;
    PointType * nodes;
    long numOfItem=(BUFFERSIZE-sizeof(PointType *))/sizeof(PointType *);
    long n;
    int step=STEP/sizeof(PointType *);
    nodes=(PointType *)malloc(numOfItem*sizeof(PointType *));
    n=0;

    t=nowTimeInSec();
    for (int i=0; i<numOfItem; i++) {
        nodes[n]=(PointType)(nodes+(n+step)%numOfItem);
        n=(n+step)%numOfItem;
    }
    t=nowTimeInSec()-t;
    printf("Write Latency: Run %.2lfs,with %.4lfMT/s, %.4lfns latency\n",t,numOfItem/t/1.e6,t*1.e9/numOfItem);

    t=nowTimeInSec();

    t=nowTimeInSec()-t;
//    printf("%i\n",nw->a[0]);
    printf("Read Latency: Run %.2lfs,with %.4lfMT/s, %.4lfns latency\n",t,ITERNUM/t/1.e6,t*1.e9/ITERNUM);
    
    return 0;
}