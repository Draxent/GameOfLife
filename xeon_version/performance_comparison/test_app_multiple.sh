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

#!/usr/bin/env bash

# VARIABLES TO SET
WORKING_DIR=~/Project

# DERIVATE VARIABLES
PERFORM_DIR=$WORKING_DIR/performance_comparison
BUILD_DIR=$WORKING_DIR/build

DATE=$(date +%y-%m-%d)

mkdir PERFORM_DIR/DATE

TIME=$(date +%s%N | cut -b1-13)

# Low Level Threading Mechanisms
$PERFORM_DIR/test_app.sh $TIME 5000 "false" "false" "DATE/perf_thread_25M.txt"
$PERFORM_DIR/test_app.sh $TIME 10000 "false" "false" "DATE/perf_thread_100M.txt"

# Low Level Threading Mechanisms + Vectorization
$PERFORM_DIR/test_app.sh $TIME 5000 "true" "false" "DATE/perf_vect_thread_25M.txt"
$PERFORM_DIR/test_app.sh $TIME 10000 "true" "false" "DATE/perf_vect_thread_100M.txt"

# FastFlow framework
$PERFORM_DIR/test_app.sh $TIME 5000 "false" "true" "DATE/perf_ff_25M.txt"
$PERFORM_DIR/test_app.sh $TIME 10000 "false" "true" "DATE/perf_ff_100M.txt"

# FastFlow framework + Vectorization
$PERFORM_DIR/test_app.sh $TIME 5000 "true" "true" "DATE/perf_vect_ff_25M.txt"
$PERFORM_DIR/test_app.sh $TIME 10000 "true" "true" "DATE/perf_vect_ff_100M.txt"