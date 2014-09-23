#!/bin/bash
gcc-4.8 -std=c99 -O2 -pthread mem.c -o mem
echo 'sequential latency:'

./mem 2 1 1 1000 1
./mem 2 2 1 1000 1
./mem 2 4 1 1000 1
./mem 2 8 1 1000 1

echo 'random latency:'

./mem 3 1 1 1000 1
./mem 3 2 1 1000 1
./mem 3 4 1 1000 1
./mem 3 8 1 1000 1
