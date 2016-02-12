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

NUMBER='^[0-9]+$'

# The average time can be calculated for a series of numbers of length equal to or greater then 3
if [ $# -lt 3 ]; then
	echo "Usage: $0 num1 num2 num3 [other_nums]"
	exit 1
fi

# Sort Array
results=($@)
sort_results=( $(echo ${results[*]} | sed 's/ /\n/g' | sort -n) )

# Calculate sum of all components exepts extremes
sum=0
for i in $(seq 1 $(( ${#results[@]} - 2 )) )
do
	sum=$(( $sum +  ${sort_results[$i]} ))
done

#`Print the average
echo $(( ${sum}/$((${#results[@]} - 2)) ))