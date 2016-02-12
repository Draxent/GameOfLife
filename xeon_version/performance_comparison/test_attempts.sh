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
START=100
STEP=100
END=1000
NUM_VALUES=10

# DERIVATE VARIABLES
PERFORM_DIR=$WORKING_DIR/performance_comparison
SOURCE_DIR=$WORKING_DIR/attempts

if [ $# -lt 1 ]; then
	echo "Usage: $0 testtype"
	exit 1
fi

if [ $1 -eq 1 ]; then
	OUTPUT_FILE="perf_1vs2_2.txt"
	PROGS=("1.2m" "2.1m")
	OPT_BOOSTS=("false" "false")
else
	OUTPUT_FILE="perf_data_structure_2.txt"
	PROGS=("3.array_bool" "4.vector_bool" "5.array_bitset" "6.dynamic_bitset")
	OPT_BOOSTS=("false" "false" "false" "true")
fi

echo -e -n "SIDE\t#CELLS" > $PERFORM_DIR/$OUTPUT_FILE

# enter in the source directory
cd $SOURCE_DIR

i=0
for prog in ${PROGS[*]}
do
	# one copy on XEON PHI
	rm $prog
	make TARGET=$prog BOOST=${OPT_BOOSTS[$i]} MIC="true"
	scp $prog mic0:
	
	# one copy on XEON HOST
	rm $prog
	make TARGET=$prog BOOST=${OPT_BOOSTS[$i]}
	
	# print label on file
	echo -e -n "\t${prog}\t${prog}_phi" >> $PERFORM_DIR/$OUTPUT_FILE
	
	i=$(($i + 1))
done
# endline
echo "" >> $PERFORM_DIR/$OUTPUT_FILE

echo -n "Computed: "

for i in $(seq $START $STEP $END)
do
	cells=$(($i * $i))
	echo -e -n "$i\t$cells" >> $PERFORM_DIR/$OUTPUT_FILE
	
	for prog in ${PROGS[*]}
	do
		# perfomance program on XEON HOST
		for j in $(seq 1 $NUM_VALUES)
		do
			results[$j]=$(./$prog $i $STEP)
			if ! [[ ${results[$j]} =~ $NUMBER ]]; then
				echo -e "\033[1;31mError! \033[0m"
				exit 1
			fi
		done
		result1=$($PERFORM_DIR/avg_time.sh ${results[*]})
		if [ $? != 0 ]; then
			echo -e "\033[1;31mError! \033[0m"
			exit 1
		fi

		# perfomance program on XEON PHI
		for j in $(seq 1 $NUM_VALUES)
		do
			results[$j]=$(ssh mic0 ./$prog $i $STEP)
			if ! [[ ${results[$j]} =~ $NUMBER ]]; then
				echo -e "\033[1;31mError! \033[0m"
				exit 1
			fi
		done
		result2=$($PERFORM_DIR/avg_time.sh ${results[*]})
		if [ $? != 0 ]; then
			echo -e "\033[1;31mError! \033[0m"
			exit 1
		fi
		
		# print performances
		echo -e -n "\t${result1}\t${result2}" >> $PERFORM_DIR/$OUTPUT_FILE	
	done
	
	# endline
	echo "" >> $PERFORM_DIR/$OUTPUT_FILE
	# print computed input
	echo -n "$i, "
done

echo -e "\033[1;92mDone !\033[0m"