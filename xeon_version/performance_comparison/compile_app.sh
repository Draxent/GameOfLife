#!/usr/bin/env bash

# VARIABLES TO SET
WORKING_DIR=~/Project
MIC="mic1"

# DERIVATE VARIABLES
BUILD_DIR=$WORKING_DIR/build

cd $WORKING_DIR
make cleanall

# one copy on XEON PHI
make MIC="true" MACHINE_TIME="true"
scp $BUILD_DIR/GOL_thread $BUILD_DIR/GOL_ff $MIC:

# one copy on XEON HOST
make cleanall
make MACHINE_TIME="true"