#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xmmintrin.h>
#include <sys/time.h>
#include <pthread.h>

#define IOPSLOOPS 100000000000L
#define FLOPSLOOPS 1000000000L
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
    x1=_mm_set_epi32(19,4,6,2);
    y1=_mm_set_epi32(4,2,10,9);
    x2=_mm_set_epi32(29,24,26,22);
    y2=_mm_set_epi32(24,22,20,29);
    x3=_mm_set_epi32(39,34,36,32);
    y3=_mm_set_epi32(34,32,30,39);
    x4=_mm_set_epi32(49,44,46,24);
    y4=_mm_set_epi32(44,42,10,49);
    x5=_mm_set_epi32(59,54,56,52);
    y5=_mm_set_epi32(54,52,50,59);
    x6=_mm_set_epi32(69,64,66,64);
    y6=_mm_set_epi32(64,62,60,69);
    
    for (long i=0; i<IOPSLOOPS/24; i++) {
        x1=_mm_add_epi32(x1, y1);
        x2=_mm_add_epi32(x2, y2);
        x3=_mm_add_epi32(x3, y3);
        x4=_mm_add_epi32(x4, y4);
        x5=_mm_add_epi32(x5, y5);
        x6=_mm_add_epi32(x6, y6);
    }
    int *a=(int*)malloc(sizeof(int)*4);
    a=(int*)(&x1);
    printf("%d,%d,%d,%d\n",a[0],a[1],a[2],a[3]);
    a=(int*)(&x2);
    printf("%d,%d,%d,%d\n",a[0],a[1],a[2],a[3]);
    a=(int*)(&x3);
    printf("%d,%d,%d,%d\n",a[0],a[1],a[2],a[3]);
    a=(int*)(&x4);
    printf("%d,%d,%d,%d\n",a[0],a[1],a[2],a[3]);
    a=(int*)(&x5);
    printf("%d,%d,%d,%d\n",a[0],a[1],a[2],a[3]);
    a=(int*)(&x6);
    printf("%d,%d,%d,%d\n",a[0],a[1],a[2],a[3]);
    pthread_exit(NULL);
}

int main(int argc, char** argv) {

    long n=IOPSLOOPS;
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
    }
    for (int i=0; i<numOfThread; i++) {
        pthread_join(threads[i], NULL);
    }

    t=nowTime()-t;
    printf("time:\t %.3f s, %.3f Gflops\n",t,(double)n*NUMOFTHREAD/t/1e9);
    
    pthread_attr_destroy(&attr);
    free(threads);
    return EXIT_SUCCESS;
}