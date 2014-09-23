#!/bin/bash
gcc -std=c99 -O2 -pthread cpu.c -o cpu
rm ./cpu.log
touch ./cpu.log
for testType in 0 1
do
    for threadNum in 1 2 4 8
    do
        for t in 1 2 3 4 5
        do
            ./cpu $testType $threadNum $100000 >> ./cpu.log
        done
    done
done


gcc -std=c99 -O2 -pthread mem.c -o mem
rm ./mem.log
touch ./mem.log
for testType in 0 1
do
    for threadNum in 1 2 4 8
        do
            for b in 1 10 100
            do
                for s in b k m
                do
                    for t in 1 2 3 4 5
                    do
                        ./mem $testType  $threadNum 1000 $b $s >> ./mem.log
                    done
                done
                for t in 1 2 3 4 5
                do
                    ./mem $testType  $threadNum 1000 1 g >> ./mem.log
                done
            done
        done
done

for testType in 2 3
do
    for threadNum in 1 2 4 8
    do
        for t in 1 2 3 4 5
        do
            ./mem $testType $threadNum >> ./mem.log
        done
    done
done

