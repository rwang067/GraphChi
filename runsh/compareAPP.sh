#!/bin/bash

# 2018.12.30
# Compare with GraphChi
echo "2018.12.30 Compare with GraphChi" >> graphchi_metrics.txt.statistics 
echo "app = RWD,GraphLet,SR,PPR" >> graphchi_metrics.txt.statistics 

# ### SSD
# ################################################################################################
# ################################################################################################

## LJ
################################################################################################
# RWD, LJ
echo "SSD App = RWD, Dataset = LJ" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/rwdomination file ../dataset/LiveJournal/soc-LiveJournal1.txt nshards 5 N 4847571
done
# Graphlet, LJ
echo "SSD App = Graphlet, Dataset = LJ" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/graphlet file ../dataset/LiveJournal/soc-LiveJournal1.txt nshards 5 N 4847571
done
# PPR, LJ
echo "SSD App = PPR, Dataset = LJ" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/personalizedpagerank file ../dataset/LiveJournal/soc-LiveJournal1.txt nshards 5 source 0
done
SimRank, LJ
echo "SSD App = SimRank, Dataset = LJ" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/simrank file ../dataset/LiveJournal/soc-LiveJournal1.txt nshards 5 a 0 b 1
done
## WP
################################################################################################
# RWD, WP
echo "SSD App = RWD, Dataset = WP" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/rwdomination file ../dataset/Wikipedia/wikipedia_sorted.data nshards 31 N 12150977
done
# Graphlet, WP
echo "SSD App = Graphlet, Dataset = WP" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/graphlet file ../dataset/Wikipedia/wikipedia_sorted.data nshards 31 N 12150977
done
PPR, WP
echo "SSD App = PPR, Dataset = WP" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/personalizedpagerank file ../dataset/Wikipedia/wikipedia_sorted.data nshards 31 source 1
done
# SimRank, WP
echo "SSD App = SimRank, Dataset = WP" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/simrank file ../dataset/Wikipedia/wikipedia_sorted.data nshards 31 a 1 b 3
done
## TT
################################################################################################
# RWD, TT
echo "SSD App = RWD, Dataset = TT" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/rwdomination file ../dataset/Twitter/twitter_rv.net nshards 114 N 61578415
done
# Graphlet, TT
echo "SSD App = Graphlet, Dataset = TT" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/graphlet file ../dataset/Twitter/twitter_rv.net nshards 114 N 61578415
done
# PPR, TT
echo "SSD App = PPR, Dataset = TT" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/personalizedpagerank file ../dataset/Twitter/twitter_rv.net nshards 114 source 12
done
# SimRank, TT
echo "SSD App = SimRank, Dataset = TT" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/simrank file ../dataset/Twitter/twitter_rv.net nshards 114 a 12 b 13
done

# ### HDD
# ################################################################################################
# ################################################################################################

## LJ
################################################################################################
# RWD, LJ
echo "HDD App = RWD, Dataset = LJ" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/rwdomination file ../DataSet/LiveJournal/soc-LiveJournal1.txt nshards 5 N 4847571
done
# Graphlet, LJ
echo "HDD App = Graphlet, Dataset = LJ" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/graphlet file ../DataSet/LiveJournal/soc-LiveJournal1.txt nshards 5 N 4847571
done
# PPR, LJ
echo "HDD App = PPR, Dataset = LJ" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/personalizedpagerank file ../DataSet/LiveJournal/soc-LiveJournal1.txt nshards 5 source 0
done
# SimRank, LJ
echo "HDD App = SimRank, Dataset = LJ" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/simrank file ../DataSet/LiveJournal/soc-LiveJournal1.txt nshards 5 a 0 b 1
done
# WP
###############################################################################################
# RWD, WP
echo "HDD App = RWD, Dataset = WP" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/rwdomination file ../DataSet/Wikipedia/wikipedia_sorted.data nshards 31 N 12150977
done
# Graphlet, WP
echo "HDD App = Graphlet, Dataset = WP" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/graphlet file ../DataSet/Wikipedia/wikipedia_sorted.data nshards 31 N 12150977
done
# PPR, WP
echo "HDD App = PPR, Dataset = WP" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/personalizedpagerank file ../DataSet/Wikipedia/wikipedia_sorted.data nshards 31 source 1
done
# SimRank, WP
echo "HDD App = SimRank, Dataset = WP" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/simrank file ../DataSet/Wikipedia/wikipedia_sorted.data nshards 31 a 1 b 3
done
## TT
################################################################################################
# RWD, TT
echo "HDD App = RWD, Dataset = TT" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/rwdomination file ../DataSet/Twitter/twitter_rv.net nshards 114 N 61578415
done
# Graphlet, TT
echo "HDD App = Graphlet, Dataset = TT" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/graphlet file ../DataSet/Twitter/twitter_rv.net nshards 114 N 61578415
done
# PPR, TT
echo "HDD App = PPR, Dataset = TT" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/personalizedpagerank file ../DataSet/Twitter/twitter_rv.net nshards 114 source 12
done
SimRank, TT
echo "HDD App = SimRank, Dataset = TT" >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 5; times++))
do
    echo "times = " $times " from echo"
    ./bin/apps/simrank file ../DataSet/Twitter/twitter_rv.net nshards 114 a 12 b 13
done