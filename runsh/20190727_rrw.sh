#!/bin/bash

ulimit -n 1024000

# Compare with GraphChi -- Raw random walks
echo "2019.7.27 " >> graphchi_metrics.txt.statistics 
echo "raw random walks performance comparison (clear pagecache each time) in 64GB R730, dataset = Twitter" >> graphchi_metrics.txt.statistics 
echo "Turn on Page Cache, clear pagecache each time, nshards = 16" >> graphchi_metrics.txt.statistics 

### 64GB R730, SSD, YW
################################################################################################
echo "L = 10, different Rs" >> graphchi_metrics.txt.statistics 
for(( R = 1000; R <= 1000000000; R*=10))
do
    echo "L = 10, R = " $R >> graphchi_metrics.txt.statistics 
    for(( times = 0; times < 5; times++))
    do
        echo "times = " $times " from echo"
        free -m
        sync; echo 1 > /proc/sys/vm/drop_caches
        free -m
        ./bin/apps/rawrandomwalks file ../../raid0_defghij/Yahoo/yahoo-webmap.txt N 1413511394 R $R L 10 nshards 130
    done
done

### 64GB R730, SSD, Yw
################################################################################################
echo "R = 100000, different Ls" >> graphchi_metrics.txt.statistics 
for(( L = 4; L <= 4096; L*=2))
do
    echo "R = 100000, L = " $L >> graphchi_metrics.txt.statistics 
    for(( times = 0; times < 5; times++))
    do
        echo "times = " $times " from echo"
        free -m
        sync; echo 1 > /proc/sys/vm/drop_caches
        free -m
        ./bin/apps/rawrandomwalks file ../../raid0_defghij/Yahoo/yahoo-webmap.txt N 1413511394 R 100000 L $L nshards 130
    done
done

# Compare with GraphChi -- Raw random walks
echo "2019.7.27 " >> graphchi_metrics.txt.statistics 
echo "raw random walks performance comparison (clear pagecache each time) in 64GB R730, dataset = Twitter" >> graphchi_metrics.txt.statistics 
echo "Turn on Page Cache, clear pagecache each time, nshards = 16" >> graphchi_metrics.txt.statistics 

### 64GB R730, SSD, FS
################################################################################################
echo "L = 10, different Rs" >> graphchi_metrics.txt.statistics 
for(( R = 1000; R <= 1000000000; R*=10))
do
    echo "L = 10, R = " $R >> graphchi_metrics.txt.statistics 
    for(( times = 0; times < 5; times++))
    do
        echo "times = " $times " from echo"
        free -m
        sync; echo 1 > /proc/sys/vm/drop_caches
        free -m
        ./bin/apps/rawrandomwalks file ../../raid0_defghij/Friendster/out.friendster-reorder N 68349467 R $R L 10
    done
done

### 64GB R730, SSD, FS
################################################################################################
echo "R = 100000, different Ls" >> graphchi_metrics.txt.statistics 
for(( L = 4; L <= 4096; L*=2))
do
    echo "R = 100000, L = " $L >> graphchi_metrics.txt.statistics 
    for(( times = 0; times < 5; times++))
    do
        echo "times = " $times " from echo"
        free -m
        sync; echo 1 > /proc/sys/vm/drop_caches
        free -m
        ./bin/apps/rawrandomwalks file ../../raid0_defghij/Friendster/out.friendster-reorder N 68349467 R 100000 L $L
    done
done

ulimit -n 1024