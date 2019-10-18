#!/bin/bash

# 2019.7.5
# Compare with GraphChi -- Raw random walks
echo "2019.7.13 " >> graphchi_metrics.txt.statistics 
echo "random walks applications performance comparison (clear pagecache each time) in 64GB R730, dataset = Twitter" >> graphchi_metrics.txt.statistics 
echo "Turn on Page Cache, clear pagecache each time, nshards = 16" >> graphchi_metrics.txt.statistics 

### 64GB R730, SSD, Twitter
########################################################
echo "APP = PPR " >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/personalizedpagerank file ../../raid0_defghij/Twitter/twitter_rv.net source 12
done

### 64GB R730, SSD, Twitter
#########################################################
echo "App = SimRank" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/simrank file ../../raid0_defghij/Twitter/twitter_rv.net a 12 b 13
done

### 64GB R730, SSD, Twitter
#########################################################
echo "App = Graphlet" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/graphlet file ../../raid0_defghij/Twitter/twitter_rv.net N 61578415 R 100000
done



echo "2019.7.13 " >> graphchi_metrics.txt.statistics 
echo "random walks applications performance comparison (clear pagecache each time) in 64GB R730, dataset = Friendster" >> graphchi_metrics.txt.statistics 
echo "Turn on Page Cache, clear pagecache each time, nshards = 27" >> graphchi_metrics.txt.statistics 

### 64GB R730, SSD, Friendster
#######################################################
echo "App = PPR" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/personalizedpagerank file ../../raid0_defghij/Friendster/out.friendster-reorder source 13
done

### 64GB R730, SSD, Friendster
#########################################################
echo "App = SimRank" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/simrank file ../../raid0_defghij/Friendster/out.friendster-reorder a 12 b 13
done

### 64GB R730, SSD, Friendster
###########################################################
echo "App = Graphlet" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/graphlet file ../../raid0_defghij/Friendster/out.friendster-reorder N 68349467 R 100000
done

### 64GB R730, SSD, Friendster
###########################################################
echo "App = RandomWalkDomination" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/rwdomination file ../../raid0_defghij/Friendster/out.friendster-reorder N 68349467
done



echo "2019.7.13 " >> graphchi_metrics.txt.statistics 
echo "random walks applications performance comparison (clear pagecache each time) in 64GB R730, dataset = Yahoo" >> graphchi_metrics.txt.statistics 
echo "Turn on Page Cache, clear pagecache each time, nshards = 65" >> graphchi_metrics.txt.statistics 

### 64GB R730, SSD, Yahoo
###########################################################
echo "App = PPR" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/personalizedpagerank file ../../raid0_defghij/Yahoo/yahoo-webmap.txt source 9
done

### 64GB R730, SSD, Yahoo
############################################################
echo "App = SimRank" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/simrank file ../../raid0_defghij/Yahoo/yahoo-webmap.txt a 4 b 9
done

### 64GB R730, SSD, Yahoo
############################################################
echo "App = Graphlet" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/graphlet file ../../raid0_defghij/Yahoo/yahoo-webmap.txt N 1413511394 R 100000
done





################################################## RWD ################################################################
echo "2019.7.13 RWD " >> graphchi_metrics.txt.statistics 
################################################## RWD ################################################################


### 64GB R730, SSD, Twitter
#####################################################################
echo "App = RandomWalkDomination" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/rwdomination file ../../raid0_defghij/Twitter/twitter_rv.net N 61578415
done


echo "random walks applications performance comparison (clear pagecache each time) in 64GB R730, dataset = Friendster" >> graphchi_metrics.txt.statistics 
### 64GB R730, SSD, Friendster
######################################################################
echo "App = RandomWalkDomination" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 3; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/rwdomination file ../../raid0_defghij/Friendster/out.friendster-reorder N 68349467
done


echo "random walks applications performance comparison (clear pagecache each time) in 64GB R730, dataset = Yahoo" >> graphchi_metrics.txt.statistics 
### 64GB R730, SSD, Yahoo
######################################################################
echo "App = RandomWalkDomination" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 3; times++))
do
    echo "times = " $times " from echo"
    free -m
    sync; echo 1 > /proc/sys/vm/drop_caches
    free -m
    ./bin/apps/rwdomination file ../../raid0_defghij/Yahoo/yahoo-webmap.txt N 1413511394
done