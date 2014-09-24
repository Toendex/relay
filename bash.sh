#!/bin/bash
rm cpu
gcc -std=c99 -O2 -funroll-loops -pthread cpu.c -o cpu
rm ./cpu.log
touch ./cpu.log
for testType in 0 1
do
    for threadNum in 1 2 4 8
    do
        for t in 1 2 3 4 5
        do
            ./cpu $testType $threadNum 100000 >> ./cpu.log
        done
    done
done

rm mem
gcc -std=c99 -pthread mem.c -o mem
rm ./mem.log
touch ./mem.log
for testType in 0 1
do
    for threadNum in 1 2 4 8
        do
            for b in 1 10
            do
                for t in 1 2 3 4 5
                do
                    ./mem $testType  $threadNum 1 1000 $b b >> ./mem.log
                done
            done
            for t in 1 2 3
            do
                ./mem $testType  $threadNum 10 1000 100 b >> ./mem.log
            done
            for s in k m
            do
                for b in 1 10 100
                do
                    for t in 1 2 3 4 5
                    do
                        ./mem $testType  $threadNum 20 1000 $b $s >> ./mem.log
                    done
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

rm ml
gcc -std=c99 -pthread memLat.c -o ml
rm ./ml.log
touch ./ml.log
for threadNum in 1 2 4 8
do
    for StepInByte in 32 64 128 256 512 1024 2048
    do
        for t in 1 2 3 4 5
        do
            ./ml $threadNum $StepInByte >> ./ml.log
        done
    done
done
