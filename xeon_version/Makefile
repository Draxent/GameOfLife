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

# Control Variables
INTEL_COMPILER = true
MIC = false
DEBUG = false
MACHINE_TIME = false
TAKE_ALL_TIME = true

# Pointing to the FastFlow root directory (i.e. the one containing the ff directory).
FF_ROOT 	= /home/spm1501/public/fastflow
#~/fastflow
#/home/spm1501/public/fastflow

ifeq ($(INTEL_COMPILER),true)
	CXX = icpc -D VECTORIZATION
	 #-ipo
	ifeq ($(MIC),true)
    	XEONPHI = -mmic -D MIC -DNO_DEFAULT_MAPPING
    endif
else
	CXX = g++
endif

ifeq ($(DEBUG),true)
	OPTFLAGS = -g -D DEBUG
else
	ifeq ($(MACHINE_TIME),true)
		MACHINE_FLAG = -D MACHINE_TIME
	endif
	OPTFLAGS = # -O3 -finline-functions -DNDEBUG $(MACHINE_FLAG)
endif

# Compiler & Libs
CXX_FLAGS	= -std=c++11 $(XEONPHI) $(OPTFLAGS)
LDFLAGS 	= -pthread

.PHONY: all clean clean_thread clean_ff cleanall

all: build/GOL_thread build/GOL_ff

build/GOL_thread: src/main_thread.cpp build/grid.o build/program_options.o build/shared_functions.o build/matrix.o
	$(CXX) $(CXX_FLAGS) src/main_thread.cpp build/grid.o build/program_options.o build/shared_functions.o build/matrix.o -o $@ $(LDFLAGS)
	@echo "Compiled $@ successfully!"

build/GOL_ff: src/main_ff.cpp build/grid.o build/program_options.o build/shared_functions.o build/master.o build/worker.o build/matrix.o
	$(CXX) $(CXX_FLAGS) -I $(FF_ROOT) src/main_ff.cpp build/grid.o build/program_options.o build/shared_functions.o build/master.o build/worker.o build/matrix.o include/task.h -o $@ $(LDFLAGS)
	@echo "Compiled $@ successfully!"

build/grid.o : src/grid.cpp include/grid.h
	$(CXX) $(CXX_FLAGS) -c $< -o $@
	@echo "Compiled $< successfully!"

build/master.o : src/master.cpp include/master.h
	$(CXX) $(CXX_FLAGS) -I $(FF_ROOT) -c $< -o $@
	@echo "Compiled $< successfully!"

build/matrix.o : src/matrix.cpp include/matrix.h
	$(CXX) $(CXX_FLAGS) -c $< -o $@
	@echo "Compiled $< successfully!"

build/program_options.o : src/program_options.cpp include/program_options.h
	$(CXX) $(CXX_FLAGS) -c $< -o $@
	@echo "Compiled $< successfully!"

build/shared_functions.o : src/shared_functions.cpp include/shared_functions.h
	$(CXX) $(CXX_FLAGS) -c $< -o $@
	@echo "Compiled $< successfully!"

build/worker.o : src/worker.cpp include/worker.h
	$(CXX) $(CXX_FLAGS) -I $(FF_ROOT) -c $< -o $@
	@echo "Compiled $< successfully!"

clean_thread:
	rm -f build/GOL_thread
	@echo "Cleanup build/GOL_thread completed!"

clean_ff:
	rm -f build/GOL_ff
	@echo "Cleanup build/GOL_ff completed!"

clean:
	rm -f build/GOL_thread build/GOL_ff
	@echo "Cleanup completed!"

cleanall:
	rm -f build/*~ build/*.o build/GOL_thread build/GOL_ff
	@echo "Cleanup all completed!"