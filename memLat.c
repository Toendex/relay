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
#define NUMOFNODE 100000000

struct node {
    char a[10];
    struct node *next;
};

double nowTimeInSec() {
    struct timeval t;
    gettimeofday(&t,NULL);
    return (double)t.tv_sec+t.tv_usec/1000000.0;
}

int main(int argc, const char * argv[])
{
    double t;
    
    struct node *nd=(struct node*)malloc(sizeof(struct node));
    struct node *root=nd;
    struct node *nw;
    struct node **nodes=(struct node**)malloc(sizeof(struct node*)*NUMOFNODE);
    for (int i=0; i<NUMOFNODE; i++) {
        nodes[i]=(struct node*)malloc(sizeof(struct node));
    }
    
    nw=root;
    t=nowTimeInSec();
    for (int i=0; i<NUMOFNODE; i++) {
        nw->next=nodes[i];
        nw=nw->next;
    }
    t=nowTimeInSec()-t;
    printf("Write Latency: Run %.2lfs,with %.4lfMT/s, %.4lfns latency\n",t,NUMOFNODE/t/1.e6,t*1.e9/NUMOFNODE);
    nw->next=root;
    
    nw=root;
    t=nowTimeInSec();
    for (long i=0; i<ITERNUM; i++) {
        nw=nw->next;
    }
    t=nowTimeInSec()-t;
    printf("%i\n",nw->a[0]);
    printf("Read Latency: Run %.2lfs,with %.4lfMT/s, %.4lfns latency\n",t,ITERNUM/t/1.e6,t*1.e9/ITERNUM);
    

    
    return 0;
}