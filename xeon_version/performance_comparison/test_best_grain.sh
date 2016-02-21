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
NUM_VALUES=5
ITERATIONS=100

# DERIVATE VARIABLES
PERFORM_DIR=$WORKING_DIR/performance_comparison
BUILD_DIR=$WORKING_DIR/build

if [ $# -lt 10 ]; then
	echo "Usage: $0 mic seed side start_grain step_grain stop_grain nw vect ff output"
	exit 1
fi

MIC=$1
SEED=$2
SIDE=$3
START_GRAIN=$4
STEP_GRAIN=$5
STOP_GRAIN=$6
NW=$7
VECTORIZATION=$8
FASTFLOW_VERSION=$9
OUTPUT_FILE=${10}

echo "TEST: MIC: $MIC, SEED: $SEED, SIDE: $SIDE, START_GRAIN: $START_GRAIN, STEP_GRAIN: $STEP_GRAIN, STOP_GRAIN: $STOP_GRAIN, NW: $NW, VECT: $VECTORIZATION, FF:$FASTFLOW_VERSION, OUTPUT_FILE:$OUTPUT_FILE."
echo "TEST: MIC: $MIC, SEED: $SEED, SIDE: $SIDE, START_GRAIN: $START_GRAIN, STEP_GRAIN: $STEP_GRAIN, STOP_GRAIN: $STOP_GRAIN, NW: $NW, VECT: $VECTORIZATION, FF:$FASTFLOW_VERSION, OUTPUT_FILE:$OUTPUT_FILE." > $PERFORM_DIR/$OUTPUT_FILE

if [ "$VECTORIZATION" = "true" ]; then VECT="-v"; fi
if [ "$FASTFLOW_VERSION" = "true" ]; then TARGET="GOL_ff"; else TARGET="GOL_thread"; fi

###################################
# perfomance program on XEON HOST #
###################################
echo -e "GRAIN\tCOMPLETE GOL" >> $PERFORM_DIR/$OUTPUT_FILE
echo -n "Computed: "

iter=(0 $(seq $START_GRAIN $STEP_GRAIN $STOP_GRAIN))
for grain in ${iter[@]}
do
	echo -n "$grain" >> $PERFORM_DIR/$OUTPUT_FILE

    for j in $(seq 1 $NUM_VALUES)
    do
    	if [ "$MIC" = "NULL" ]; then
    		output=$($BUILD_DIR/$TARGET $VECT -s $SEED -g $grain -w $SIDE -h $SIDE -i $ITERATIONS -t $NW)
    	else
    		output=$(ssh $MIC ./$TARGET $VECT -s $SEED -g $grain -w $SIDE -h $SIDE -i $ITERATIONS -t $NW)
    	fi
        gol[$j]=$(echo $output | sed 's/T/\nT/g' | grep "complete Game of Life" | tr -dc '0-9')
        if ! [[ ${gol[$j]} =~ $NUMBER ]]; then echo -e "\033[1;31mError GOL! \033[0m"; exit 1; fi
    done
    gol=$($PERFORM_DIR/avg_time.sh ${gol[@]})
    if [ $? != 0 ]; then echo -e "\033[1;31mError GOL avg_time.sh! \033[0m"; exit 1; fi

	# print performances
	echo -e "\t$gol" >> $PERFORM_DIR/$OUTPUT_FILE
	# print computed input
	echo -n "$grain, "
done

echo -e "\033[1;92mDone !\033[0m"
echo -e  "\033[1;92mTEST CORRECTLY COMPLETED !\033[0m"