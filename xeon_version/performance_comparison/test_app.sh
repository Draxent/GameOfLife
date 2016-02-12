#  -----------------------------------------------------------------------------------
#	Author: Federico Conte (draxent)
#
#	Copyright 2015 Federico Conte
#	https://github.com/Draxent/GameOfLife
#
#	Licensed under the Apache License, Version 2.0 (the "License");
#	you may not use this file except in compliance with the License.
#	You may obtain a copy of the License at
#	
#	http://www.apache.org/licenses/LICENSE-2.0
#
#	Unless required by applicable law or agreed to in writing, software
#	distributed under the License is distributed on an "AS IS" BASIS,
#	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#	See the License for the specific language governing permissions and
#	limitations under the License.
# -----------------------------------------------------------------------------------

#!/bin/bash

# FIXED VARIABLES
NUMBER='^[0-9]+$'

# VARIABLES TO SET
WORKING_DIR=~/Project
SIDE=1000
STEPS=100
OUTPUT_FILE="perf_thread_1M.txt"
TARGET="game_of_life"
NUM_VALUES=10
FASTFLOW_VERSION="false"

# DERIVATE VARIABLES
PERFORM_DIR=$WORKING_DIR/performance_comparison
BUILD_DIR=$WORKING_DIR/build

echo -e "NUM_THREADS\tXEON HOST\t\t\t\t\t\t\tXEON PHI\t\t\t\t\t\t" > $PERFORM_DIR/$OUTPUT_FILE
echo -w "\t\tINIT PHASE\tCREATING THREADS\tBARRIER PHASE\tCOMPLETE GOL\tINIT PHASE\tCREATING THREADS\tBARRIER PHASE\tCOMPLETE GOL" > $PERFORM_DIR/$OUTPUT_FILE

cd $WORKING_DIR
make cleanall

# one copy on XEON PHI
make MIC="true" MACHINE_TIME="true"
scp $BUILD_DIR/$TARGET mic0:

# one copy on XEON HOST
make cleanall
make MACHINE_TIME="true"

echo -n "Computed: "

if [ "$FASTFLOW_VERSION" = "true" ]; then FF="-ff"; fi

for numthreads in $(seq 1 240)
do
	echo -n "$numthreads" >> $PERFORM_DIR/$OUTPUT_FILE
	
	if [ $numthreads -le 16 ]; then
		# perfomance program on XEON HOST
		for j in $(seq 1 $NUM_VALUES)
		do
			output=$($BUILD_DIR/$TARGET $FF -w $SIDE -h $SIDE -s $STEPS -t $numthreads)
			
			init[$j]=$(echo $output | sed 's/T/\nT/g' | grep "initialization phase" | tr -dc '0-9')
			if ! [[ ${init[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError init! \033[0m"; exit 1; fi
			
			if [ $numthreads -gt 1 ]; then
				thread[$j]=$(echo $output | sed 's/T/\nT/g' | grep "creating threads" | tr -dc '0-9')
				if ! [[ ${thread[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError thread! \033[0m"; exit 1; fi
			fi
			
			barrier[$j]=$(echo $output | sed 's/T/\nT/g' | grep "barrier phase" | tr -dc '0-9')
			if ! [[ ${barrier[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError barrier! \033[0m"; exit 1; fi
			
			gol[$j]=$(echo $output | sed 's/T/\nT/g' | grep "complete Game of Life" | tr -dc '0-9')
			if ! [[ ${gol[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError GOL! \033[0m"; exit 1; fi
		done
		
		init1=$($PERFORM_DIR/avg_time.sh ${init[@]})
		if [ $? != 0 ]; then echo -e "\033[1;31mError init avg_time.sh! \033[0m"; exit 1; fi
		if [ $numthreads != 1 ]; then
			thread1=$($PERFORM_DIR/avg_time.sh ${thread[@]})
			if [ $? != 0 ]; then echo -e "\033[1;31mError thread avg_time.sh! \033[0m"; exit 1; fi
		else
			thread1=0
		fi
		barrier1=$($PERFORM_DIR/avg_time.sh ${barrier[@]})
		if [ $? != 0 ]; then echo -e "\033[1;31mError barrier avg_time.sh! \033[0m"; exit 1; fi
		gol1=$($PERFORM_DIR/avg_time.sh ${gol[@]})
		if [ $? != 0 ]; then echo -e "\033[1;31mError GOL avg_time.sh! \033[0m"; exit 1; fi
	fi

	# perfomance program on XEON PHI
	for j in $(seq 1 $NUM_VALUES)
	do
		output=$(ssh mic0 ./$TARGET $FF -w $SIDE -h $SIDE -s $STEPS -t $numthreads)
		
		init[$j]=$(echo $output | sed 's/T/\nT/g' | grep "initialization phase" | tr -dc '0-9')
		if ! [[ ${init[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError XEON PHI init! \033[0m"; exit 1; fi
		
		if [ $numthreads -gt 1 ]; then
			thread[$j]=$(echo $output | sed 's/T/\nT/g' | grep "creating threads" | tr -dc '0-9')
			if ! [[ ${thread[$j]} =~ $NUMBER ]]; then  echo -e "\033[1;31mError XEON PHI thread! \033[0m"; exit 1; fi
		fi
		
		barrier[$j]=$(echo $output | sed 's/T/\nT/g' | grep "barrier phase" | tr -dc '0-9')
		if ! [[ ${barrier[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError XEON PHI barrier! \033[0m"; exit 1; fi
		
		gol[$j]=$(echo $output | sed 's/T/\nT/g' | grep "complete Game of Life" | tr -dc '0-9')
		if ! [[ ${gol[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError XEON PHI GOL! \033[0m"; exit 1; fi
	done
	
	init2=$($PERFORM_DIR/avg_time.sh ${init[@]})
	if [ $? != 0 ]; then echo -e "\033[1;31mError XEON PHI init avg_time.sh! \033[0m"; exit 1; fi
	if [ $numthreads != 1 ]; then
		thread2=$($PERFORM_DIR/avg_time.sh ${thread[@]})
		if [ $? != 0 ]; then echo -e "\033[1;31mError XEON PHI thread avg_time.sh! \033[0m"; exit 1; fi
	else
		thread2=0
	fi
	barrier2=$($PERFORM_DIR/avg_time.sh ${barrier[@]})
	if [ $? != 0 ]; then echo -e "\033[1;31mError XEON PHI barrier avg_time.sh! \033[0m"; exit 1; fi
	gol2=$($PERFORM_DIR/avg_time.sh ${gol[@]})
	if [ $? != 0 ]; then echo -e "\033[1;31mError XEON PHI GOL avg_time.sh! \033[0m"; exit 1; fi
	
	# print performances
	echo -e "\t$init1\t$thread1\t$barrier1\t$gol1\t$init2\t$thread2\t$barrier2\t$gol2" >> $PERFORM_DIR/$OUTPUT_FILE
	
	# print computed input
	echo -n "$numthreads, "
done

echo -e "\033[1;92mDone !\033[0m"