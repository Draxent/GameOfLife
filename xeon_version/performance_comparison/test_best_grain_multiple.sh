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
MIC="mic1"
SIZE=5000

# DERIVATE VARIABLES
PERFORM_DIR=$WORKING_DIR/performance_comparison
BUILD_DIR=$WORKING_DIR/build

DATE=$(date +%y-%m-%d)
DIR=${DATE}_grain

mkdir $PERFORM_DIR/$DIR

TIME=$(date +%s)

# Compile
$PERFORM_DIR/compile_app.sh

# Low Level Threading Mechanisms
$PERFORM_DIR/test_best_grain.sh "NULL" $TIME $SIZE 6000 1000 10000 8 "false" "false" "$DIR/thread_host.txt"
$PERFORM_DIR/test_best_grain.sh $MIC $TIME $SIZE 26000 2000 34000 120 "false" "false" "$DIR/thread_phi.txt"

# FastFlow framework
$PERFORM_DIR/test_best_grain.sh "NULL" $TIME $SIZE 6000 1000 10000 8 "false" "true" "$DIR/ff_host.txt"
$PERFORM_DIR/test_best_grain.sh $MIC $TIME $SIZE 26000 2000 34000 120 "false" "true" "$DIR/ff_phi.txt"