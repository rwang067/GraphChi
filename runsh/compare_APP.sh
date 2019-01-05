#!/bin/bash

# 2019.1.2
# Compare with GraphChi -- Raw random walks
echo "2018.1.2 Compare with GraphChi -- Raw random walks" >> graphchi_metrics.txt.statistics 
echo "Different walk number R" >> graphchi_metrics.txt.statistics 

# WP
##################################################################
echo "HDD WP L=10, vary R" >> graphchi_metrics.txt.statistics 
for(( R = 10000; R <= 10000000; R*=10 ))
do
    echo "HDD WP L=10, R=" $R >> graphchi_metrics.txt.statistics 
    for(( times = 0; times < 3; times++))
    do
        echo "times = " $times " from echo"
        ./bin/apps/graphlet file ../DataSet/Wikipedia/wikipedia_sorted.data nshards 31 nshards 31 N 12150977 L 10 R $R
    done
done

echo "HDD WP R=10000, vary L" >> graphchi_metrics.txt.statistics 
for(( L = 32; L <= 4096; L*=2))
do
    echo "HDD WP R=10000, L=" $L >> graphchi_metrics.txt.statistics 
    for(( times = 0; times < 3; times++))
    do
        echo "times = " $times " from echo"
        ./bin/apps/graphlet file ../DataSet/Wikipedia/wikipedia_sorted.data nshards 31 nshards 31 N 12150977 L $L R 10000
    done
done

# LJ
##################################################################
echo "HDD LJ L=10, vary R" >> graphchi_metrics.txt.statistics 
for(( R = 1000; R <= 10000000; R*=10))
do
    echo "HDD LJ L=10, R=" $R >> graphchi_metrics.txt.statistics 
    for(( times = 0; times < 3; times++))
    do
        echo "times = " $times " from echo"
        ./bin/apps/graphlet file ../dataset/LiveJournal/soc-LiveJournal1.txt nshards 5 N 4847571 L 10 R $R
    done
done

echo "HDD LJ R=10000, vary L" >> graphchi_metrics.txt.statistics 
for(( L = 2; L <= 4096; L*=2))
do
    echo "HDD LJ R=10000, L=" $L >> graphchi_metrics.txt.statistics 
    for(( times = 0; times < 3; times++))
    do
        echo "times = " $times " from echo"
        ./bin/apps/graphlet file ../dataset/LiveJournal/soc-LiveJournal1.txt nshards 5 N 4847571 L $L R 10000
    done
done

# TT
##################################################################
echo "HDD TT L=10, vary R" >> graphchi_metrics.txt.statistics 
for(( R = 1000; R <= 10000000; R*=10))
do
    echo "HDD TT L=10, R=" $R >> graphchi_metrics.txt.statistics 
    for(( times = 0; times < 3; times++))
    do
        echo "times = " $times " from echo"
        ./bin/apps/graphlet file ../dataset/Twitter/twitter_rv.net nshards 114 N 61578415 L 10 R $R
    done
done

echo "HDD TT R=10000, vary L" >> graphchi_metrics.txt.statistics 
for(( L = 2; L <= 4096; L*=2))
do
    echo "HDD TT R=10000, L=" $L >> graphchi_metrics.txt.statistics 
    for(( times = 0; times < 3; times++))
    do
        echo "times = " $times " from echo"
        ./bin/apps/graphlet file ../dataset/Twitter/twitter_rv.net nshards 114 N 61578415 L $L R 10000
    done
done