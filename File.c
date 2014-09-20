#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xmmintrin.h>
#include <sys/time.h>
#include <pthread.h>

#define IOPSLOOPS 100000000000L
#define FLOPSLOOPS 10000000000L
#define NUMOFTHREAD 1
#define TESTTYPE 0          //0=iops, 1=flops

double nowTime() {
    struct timeval t;
    gettimeofday(&t,NULL);
    return (double)t.tv_sec+t.tv_usec/1000000.0;
}

void * calIOPS(void *param)
{
    __m128i x1,y1;
    __m128i x2,y2;
    __m128i x3,y3;
    __m128i x4,y4;
    __m128i x5,y5;
    __m128i x6,y6;
    __m128i x7,y7;
    __m128i x8,y8;
    x1=_mm_set_epi32(19,4,6,2);
    y1=_mm_set_epi32(4,2,10,9);
    x2=_mm_set_epi32(129,424,226,122);
    y2=_mm_set_epi32(824,22,620,829);
    x3=_mm_set_epi32(39,234,936,432);
    y3=_mm_set_epi32(134,32,630,239);
    x4=_mm_set_epi32(49,244,46,324);
    y4=_mm_set_epi32(344,542,10,849);
    x5=_mm_set_epi32(559,554,456,952);
    y5=_mm_set_epi32(54,252,50,259);
    x6=_mm_set_epi32(269,364,566,64);
    y6=_mm_set_epi32(64,62,360,669);
    x7=_mm_set_epi32(379,874,276,678);
    y7=_mm_set_epi32(171,379,172,73);
    x8=_mm_set_epi32(81,484,786,484);
    y8=_mm_set_epi32(984,582,780,189);
    
    for (long i=0; i<IOPSLOOPS/32; i++) {
        x1=_mm_add_epi32(x1, y1);
        x2=_mm_add_epi32(x2, y2);
        x3=_mm_add_epi32(x3, y3);
        x4=_mm_add_epi32(x4, y4);
        x5=_mm_add_epi32(x5, y5);
        x6=_mm_add_epi32(x6, y6);
        x7=_mm_add_epi32(x7, y7);
        x8=_mm_add_epi32(x8, y8);
    }
    x1=_mm_add_epi32(x1, x2);
    x1=_mm_add_epi32(x1, x3);
    x1=_mm_add_epi32(x1, x4);
    x1=_mm_add_epi32(x1, x5);
    x1=_mm_add_epi32(x1, x6);
    x1=_mm_add_epi32(x1, x7);
    x1=_mm_add_epi32(x1, x8);
    int *a;
    a=(int*)(&x1);
    printf("%d,%d,%d,%d\n",a[0],a[1],a[2],a[3]);
    pthread_exit(NULL);
}

void * calFLOPS(void *param)
{
    __m128d x1,y1;
    __m128d x2,y2;
    __m128d x3,y3;
    __m128d x4,y4;
    __m128d x5,y5;
    __m128d x6,y6;
    __m128d x7,y7;
    __m128d x8,y8;
    
    long total=FLOPSLOOPS;
    long rLoops=total/1000000L;
    
    x1=_mm_set_pd(1.11, 2.42);
    y1=_mm_set_pd(1+1.e-8,1+1.e-8);
    x2=_mm_set_pd(3.14,-8.41);
    y2=_mm_set_pd(1+2.e-8,1+2.e-8);
    x3=_mm_set_pd(10.98,22.11);
    y3=_mm_set_pd(1+3.e-8,1+3.e-8);
    x4=_mm_set_pd(192.1,1.009);
    y4=_mm_set_pd(1+4.e-8,1+4.e-8);
    x5=_mm_set_pd(-14.03,9.078);
    y5=_mm_set_pd(1+5.e-8,1+5.e-8);
    x6=_mm_set_pd(103.7322,-13.901);
    y6=_mm_set_pd(1+6.e-8,1+6.e-8);
    x7=_mm_set_pd(81.321,9.01);
    y7=_mm_set_pd(1+7.e-8,1+7.e-8);
    x8=_mm_set_pd(-99.087,-3.108);
    y8=_mm_set_pd(1+8.e-8,1+8.e-8);
    

    for (long i=0; i<FLOPSLOOPS/4; i++) {
        x1=_mm_mul_pd(x1, y1);
        x2=_mm_mul_pd(x2, y2);
        x3=_mm_mul_pd(x3, y3);
        x4=_mm_mul_pd(x4, y4);
        x5=_mm_mul_pd(x5, y5);
        x6=_mm_mul_pd(x6, y6);
        x7=_mm_mul_pd(x1, y1);
        x8=_mm_mul_pd(x2, y2);
    }
    x1=_mm_add_pd(x1, x2);
    x1=_mm_add_pd(x1, x3);
    x1=_mm_add_pd(x1, x4);
    x1=_mm_add_pd(x1, x5);
    x1=_mm_add_pd(x1, x6);
    x1=_mm_add_pd(x1, x7);
    x1=_mm_add_pd(x1, x8);
    double *a;
    a=(double*)(&x1);
    printf("%lf,%lf\n",a[0],a[1]);
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    
    long iosLoops=IOPSLOOPS;
    long flopsLoops=FLOPSLOOPS;
    int numOfThread;
    int testType;
    pthread_attr_t attr;
    pthread_t *threads;
    
    numOfThread=NUMOFTHREAD;
    testType=TESTTYPE;
    threads=(pthread_t*)malloc(sizeof(pthread_t)*numOfThread);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    

    double t=nowTime();
    for (int i=0; i<numOfThread; i++) {
        if (testType==0)
            pthread_create(threads+i, &attr, calIOPS, NULL);
        else if(testType==1)
            pthread_create(threads+i, &attr, calFLOPS, NULL);
    }
    for (int i=0; i<numOfThread; i++) {
        pthread_join(threads[i], NULL);
    }

    t=nowTime()-t;
    if(testType==0)
        printf("time:\t %.3f s, %.3f Gilops\n",t,(double)iosLoops*NUMOFTHREAD/t/1e9);
    else if(testType==1)
        printf("time:\t %.3f s, %.3f Gflops\n",t,(double)flopsLoops*NUMOFTHREAD/t/1e9);
    
    pthread_attr_destroy(&attr);
    free(threads);
    return EXIT_SUCCESS;
}