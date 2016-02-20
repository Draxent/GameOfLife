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
ITERATIONS=100
NUM_VALUES=3

# DERIVATE VARIABLES
PERFORM_DIR=$WORKING_DIR/performance_comparison
BUILD_DIR=$WORKING_DIR/build

if [ $# -lt 6 ]; then
	echo "Usage: $0 mic seed side vect ff output"
	exit 1
fi

MIC=$1
SEED=$2
SIDE=$3
VECTORIZATION=$4
FASTFLOW_VERSION=$5
OUTPUT_FILE=$6

echo "TEST: SEED: $SEED, SIDE: $SIDE, VECT: $VECTORIZATION, FF:$FASTFLOW_VERSION, OUTPUT_FILE:$OUTPUT_FILE."

if [ "$VECTORIZATION" = "true" ]; then VECT="-v"; fi
if [ "$FASTFLOW_VERSION" = "true" ]; then TARGET="GOL_ff"; else TARGET="GOL_thread"; fi

###################################
# perfomance program on XEON HOST #
###################################
echo -e "XEON HOST" > $PERFORM_DIR/$OUTPUT_FILE
echo -e "NUM THREADS\tINIT PHASE\tCOPY BORDER\tCREATING THREADS\tBARRIER PHASE\tCOMPLETE GOL" >> $PERFORM_DIR/$OUTPUT_FILE

echo -n "XEON HOST Computed: "

for numthreads in $(seq 0 16)
do
	echo -n "$numthreads" >> $PERFORM_DIR/$OUTPUT_FILE

    for j in $(seq 1 $NUM_VALUES)
    do
        output=$($BUILD_DIR/$TARGET $VECT -s $SEED -w $SIDE -h $SIDE -i $ITERATIONS -t $numthreads)

        init[$j]=$(echo $output | sed 's/T/\nT/g' | grep "initialization phase" | tr -dc '0-9')
        if ! [[ ${init[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError init! \033[0m"; exit 1; fi

        copyborder[$j]=$(echo $output | sed 's/T/\nT/g' | grep "copy border" | tr -dc '0-9')
        if ! [[ ${copyborder[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError copyborder! \033[0m"; exit 1; fi

        if [ $numthreads -gt 0 ]; then
            thread[$j]=$(echo $output | sed 's/T/\nT/g' | grep "creating threads" | tr -dc '0-9')
            if ! [[ ${thread[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError thread! \033[0m"; exit 1; fi

            barrier[$j]=$(echo $output | sed 's/T/\nT/g' | grep "barrier phase" | tr -dc '0-9')
            if ! [[ ${barrier[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError barrier! \033[0m"; exit 1; fi
        fi

        gol[$j]=$(echo $output | sed 's/T/\nT/g' | grep "complete Game of Life" | tr -dc '0-9')
        if ! [[ ${gol[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError GOL! \033[0m"; exit 1; fi
    done

    init=$($PERFORM_DIR/avg_time.sh ${init[@]})
    if [ $? != 0 ]; then echo -e "\033[1;31mError init avg_time.sh! \033[0m"; exit 1; fi
    copyborder=$($PERFORM_DIR/avg_time.sh ${copyborder[@]})
    if [ $? != 0 ]; then echo -e "\033[1;31mError copyborder avg_time.sh! \033[0m"; exit 1; fi
    if [ $numthreads -gt 0 ]; then
        thread=$($PERFORM_DIR/avg_time.sh ${thread[@]})
        if [ $? != 0 ]; then echo -e "\033[1;31mError thread avg_time.sh! \033[0m"; exit 1; fi
        barrier=$($PERFORM_DIR/avg_time.sh ${barrier[@]})
        if [ $? != 0 ]; then echo -e "\033[1;31mError barrier avg_time.sh! \033[0m"; exit 1; fi
    else
        thread=0
        barrier=0
    fi
    gol=$($PERFORM_DIR/avg_time.sh ${gol[@]})
    if [ $? != 0 ]; then echo -e "\033[1;31mError GOL avg_time.sh! \033[0m"; exit 1; fi

	# print performances
	echo -e "\t$init\t$copyborder\t$thread\t$barrier\t$gol" >> $PERFORM_DIR/$OUTPUT_FILE

	# print computed input
	echo -n "$numthreads, "
done

echo -e "\033[1;92mDone !\033[0m"

###################################
# perfomance program on XEON PHI #
###################################
echo -e "\nXEON PHI" >> $PERFORM_DIR/$OUTPUT_FILE
echo -e "NUM THREADS\tINIT PHASE\tCOPY BORDER\tCREATING THREADS\tBARRIER PHASE\tCOMPLETE GOL" >> $PERFORM_DIR/$OUTPUT_FILE

echo -n "XEON PHI Computed: "

iter=(0 1 $(seq 10 10 240))
for numthreads in ${iter[@]}
do
	echo -n "$numthreads" >> $PERFORM_DIR/$OUTPUT_FILE

	for j in $(seq 1 $NUM_VALUES)
	do
		output=$(ssh $MIC ./$TARGET $VECT -s $SEED -w $SIDE -h $SIDE -i $ITERATIONS -t $numthreads)

		init[$j]=$(echo $output | sed 's/T/\nT/g' | grep "initialization phase" | tr -dc '0-9')
		if ! [[ ${init[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError XEON PHI init! \033[0m"; exit 1; fi

		copyborder[$j]=$(echo $output | sed 's/T/\nT/g' | grep "copy border" | tr -dc '0-9')
		if ! [[ ${copyborder[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError XEON PHI copyborder! \033[0m"; exit 1; fi

		if [ $numthreads -gt 0 ]; then
			thread[$j]=$(echo $output | sed 's/T/\nT/g' | grep "creating threads" | tr -dc '0-9')
			if ! [[ ${thread[$j]} =~ $NUMBER ]]; then  echo -e "\033[1;31mError XEON PHI thread! \033[0m"; exit 1; fi
		    barrier[$j]=$(echo $output | sed 's/T/\nT/g' | grep "barrier phase" | tr -dc '0-9')
		    if ! [[ ${barrier[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError XEON PHI barrier! \033[0m"; exit 1; fi
		fi

		gol[$j]=$(echo $output | sed 's/T/\nT/g' | grep "complete Game of Life" | tr -dc '0-9')
		if ! [[ ${gol[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError XEON PHI GOL! \033[0m"; exit 1; fi
	done

	init=$($PERFORM_DIR/avg_time.sh ${init[@]})
	if [ $? != 0 ]; then echo -e "\033[1;31mError XEON PHI init avg_time.sh! \033[0m"; exit 1; fi
	copyborder=$($PERFORM_DIR/avg_time.sh ${copyborder[@]})
	if [ $? != 0 ]; then echo -e "\033[1;31mError XEON PHI copyborder avg_time.sh! \033[0m"; exit 1; fi
	if [ $numthreads -gt 0 ]; then
		thread=$($PERFORM_DIR/avg_time.sh ${thread[@]})
		if [ $? != 0 ]; then echo -e "\033[1;31mError XEON PHI thread avg_time.sh! \033[0m"; exit 1; fi
		barrier=$($PERFORM_DIR/avg_time.sh ${barrier[@]})
	    if [ $? != 0 ]; then echo -e "\033[1;31mError XEON PHI barrier avg_time.sh! \033[0m"; exit 1; fi
	else
		thread=0
		barrier=0
	fi
	gol=$($PERFORM_DIR/avg_time.sh ${gol[@]})
	if [ $? != 0 ]; then echo -e "\033[1;31mError XEON PHI GOL avg_time.sh! \033[0m"; exit 1; fi

	# print performances
	echo -e "\t$init\t$copyborder\t$thread\t$barrier\t$gol" >> $PERFORM_DIR/$OUTPUT_FILE

	# print computed input
	echo -n "$numthreads, "
done

echo -e "\033[1;92mDone !\033[0m"
echo -e  "\033[1;92mTEST CORRECTLY COMPLETED !\033[0m"