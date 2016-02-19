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
MIC = false
DEBUG = false
MACHINE_TIME = false

ifeq ($(DEBUG),true)
	OPTFLAGS = -g -D DEBUG
else
	ifeq ($(MACHINE_TIME),true)
		MACHINE_FLAG = -D MACHINE_TIME
	endif
	OPTFLAGS = -O3 -finline-functions -DNDEBUG -ipo $(MACHINE_FLAG)
endif

ifeq ($(MIC),true)
	XEONPHI = -mmic -D MIC -DNO_DEFAULT_MAPPING
endif

# Compiler & Libs
CXX 		= icpc -std=c++11 $(XEONPHI) $(OPTFLAGS)
LDFLAGS 	= -pthread
# Pointing to the FastFlow root directory (i.e. the one containing the ff directory).
FF_ROOT 	= /home/spm1501/public/fastflow

.PHONY: all clean clean_thread clean_ff cleanall

all: build/GOL_thread build/GOL_ff

build/GOL_thread: src/main_thread.cpp build/grid.o build/grid_vect.o build/program_options.o build/shared_functions.o build/specialized_functions.o build/spinning_barrier.o build/matrix.o
	$(CXX) src/main_thread.cpp build/grid.o build/grid_vect.o build/program_options.o build/shared_functions.o build/specialized_functions.o build/spinning_barrier.o build/matrix.o -o $@ $(LDFLAGS)
	@echo "Compiled $@ successfully!"

build/GOL_ff: src/main_ff.cpp build/grid.o build/grid_vect.o build/program_options.o build/shared_functions.o build/specialized_functions.o build/spinning_barrier.o build/master.o build/worker.o build/worker_vect.o build/matrix.o
	$(CXX) -I $(FF_ROOT) src/main_ff.cpp build/grid.o build/grid_vect.o build/program_options.o build/shared_functions.o build/specialized_functions.o build/spinning_barrier.o build/master.o build/worker.o build/worker_vect.o build/matrix.o -o $@ $(LDFLAGS)
	@echo "Compiled $@ successfully!"

build/grid.o : src/grid.cpp include/grid.h
	$(CXX) -c $< -o $@
	@echo "Compiled $< successfully!"

build/grid_vect.o : src/grid_vect.cpp include/grid_vect.h
	$(CXX) -c $< -o $@
	@echo "Compiled $< successfully!"

build/master.o : src/master.cpp include/master.h
	$(CXX) -I $(FF_ROOT) -c $< -o $@
	@echo "Compiled $< successfully!"

build/matrix.o : src/matrix.cpp include/matrix.h
	$(CXX) -c $< -o $@
	@echo "Compiled $< successfully!"

build/program_options.o : src/program_options.cpp include/program_options.h
	$(CXX) -c $< -o $@
	@echo "Compiled $< successfully!"

build/shared_functions.o : src/shared_functions.cpp include/shared_functions.h
	$(CXX) -c $< -o $@
	@echo "Compiled $< successfully!"

build/specialized_functions.o : src/specialized_functions.cpp include/specialized_functions.h
	$(CXX) -c $< -o $@
	@echo "Compiled $< successfully!"

build/spinning_barrier.o : src/spinning_barrier.cpp include/spinning_barrier.h
	$(CXX) -c $< -o $@
	@echo "Compiled $< successfully!"

build/worker.o : src/worker.cpp include/worker.h
	$(CXX) -I $(FF_ROOT) -c $< -o $@
	@echo "Compiled $< successfully!"

build/worker_vect.o : src/worker_vect.cpp include/worker_vect.h
	$(CXX) -I $(FF_ROOT) -c $< -o $@
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