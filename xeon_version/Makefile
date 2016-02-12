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
VECT = true
INFO_VECT = false
DEBUG = false
MACHINE_TIME = false

ifeq ($(MIC),true)
	XEONPHI = -mmic -D MIC
endif

ifeq ($(VECT),true)
	ifeq ($(INFO_VECT),true)
		VECTORIZATION = -opt-report-phase:vec -opt-report-file=stderr -qopt-report=5
	endif
else
	VECTORIZATION = -no-vec
endif

ifeq ($(DEBUG),true)
	OPTFLAGS = -g -D DEBUG
else
	ifeq ($(MACHINE_TIME),true)
		MACHINE_FLAG = -D MACHINE_TIME
	endif
	OPTFLAGS = -O3 -finline-functions -DNDEBUG -ipo $(MACHINE_FLAG)
endif
	
# Project Name
TARGET   	= game_of_life

# Compiler & Libs
CXX		= icpc $(XEONPHI) -std=c++11 $(VECTORIZATION) -DNO_DEFAULT_MAPPING
LDFLAGS 	= -pthread

# Pointing to the FastFlow root directory (i.e. the one containing the ff directory).
FF_ROOT 	= /home/spm1501/public/fastflow

# Directories
INCLUDE_DIR	= include
SOURCE_DIR	= src
BUILD_DIR	= build

# Files
INCLUDES	= $(wildcard $(INCLUDE_DIR)/*.hpp)
SOURCES		= $(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS		= $(INCLUDES:$(INCLUDE_DIR)/%.hpp=$(BUILD_DIR)/%.o)

.PHONY: all clean cleanall

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(SOURCE_DIR)/main.cpp $(OBJECTS)
	$(CXX) $(OPTFLAGS) -I $(INCLUDE_DIR) -I $(FF_ROOT) $(SOURCE_DIR)/main.cpp $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Compiled $@ successfully!"

$(OBJECTS): $(BUILD_DIR)/%.o : $(SOURCE_DIR)/%.cpp $(INCLUDE_DIR)/%.hpp
	@$(CXX) $(OPTFLAGS) -I $(INCLUDE_DIR) -I $(FF_ROOT) -c $< -o $@
	@echo "Compiled $< successfully!"

clean:
	rm -f $(BUILD_DIR)/$(TARGET)
	@echo "Cleanup complete!"

cleanall:
	rm -f $(BUILD_DIR)/*~ $(BUILD_DIR)/*.o $(BUILD_DIR)/$(TARGET)
	@echo "Cleanup complete!"