#!/bin/bash

echo "utilizations " >> graphchi_metrics.txt.statistics 
for(( times = 0; times < 30; times++))
do
    echo "times = " $times " from echo"
    echo "times = " $times >> graphchi_metrics.txt.statistics 
    echo "times = " $times >> GraphChi_rwd_utilization.csv 
    ./bin/apps/rwdomination file ../dataset/Twitter/twitter_rv.net nshards 117 N 61578415
done