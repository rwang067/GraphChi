#!/bin/bash

app=$1

ret=$(pidof ${app})
echo ${app}" : "${ret}

outputfilename=`date '+%Y%m%d%H%M%S'`.csv
echo "APP, time, VIRT, RES, SHR, %cpu, %memory, TIME+" >> ${outputfilename}
while(true)
do
	echo ${app} ", "`date '+%Y-%m-%d %H:%M:%S'` ", " `top -n 1 -p ${ret} | grep ${ret} | awk '{print $6"\t"$7"\t"$8"\t"$10"\t"$11"\t"$12}'` >>  ${outputfilename}
	sleep 2
	if [[ `ps -ef |grep -v grep |grep -v monitor |grep ${app} | wc -l` -eq 0 ]]; then
		break
	fi
done
