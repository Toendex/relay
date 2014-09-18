//
//  File.c
//  CS553-assignment1
//
//  Created by T on 9/15/14.
//  Copyright (c) 2014 T. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <memory.h>

#define ARRAYLENGTH 6
#define LOOPS 1.e9L

double nowTime() {
    struct timeval t;
    gettimeofday(&t,NULL);
    return (double)t.tv_sec+t.tv_usec/1000000.0;
}

int main()
{
    
    double t;
    int a[ARRAYLENGTH];
    srand(time(NULL));
    for(int i=0;i<ARRAYLENGTH;i++) {
        a[i]=rand();
    }
    t=nowTime();
    for (long i=0; i<LOOPS; i++) {
        for (int j=0; j<ARRAYLENGTH-1; j++) {
            a[j]+=a[j+1];
        }
    }
    t=nowTime()-t;
    
    for (int j=0; j<ARRAYLENGTH-1; j++)
        printf("%d\t",a[j]);
    printf("\n%lfs : %lfGiops\n",t,(double)LOOPS*(ARRAYLENGTH-1)*1.e-9/t);
    return 0;
}