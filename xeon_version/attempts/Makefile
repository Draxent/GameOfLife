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

# VARIABLES
BOOST_ROOT = /home/spm1501/public/boost_1_49_0
INTEL_COMPILER = true
FASTFLOW = false
BOOST = false
MIC = false
DEBUG = false
NAME = $@

ifeq ($(INTEL_COMPILER),true)
	CXX = icpc
	ifeq ($(MIC),true)
    	XEONPHI = -mmic -DNO_DEFAULT_MAPPING
    endif
else
	CXX = g++
endif

ifeq ($(BOOST),true)
	BOOST_INCLUDE = -I $(BOOST_ROOT) -L $(BOOST_ROOT)/stage/lib
endif

ifeq ($(DEBUG),true)
	OPTFLAGS = -g -D DEBUG
else
        OPTFLAGS = -O3 -finline-functions -DNDEBUG
endif

CXX_FLAG = -std=c++11 $(XEONPHI) $(OPTFLAGS)
LDFLAGS  = -pthread

.SUFFIXES: .cpp 

$(TARGET): $(TARGET).cpp
	$(CXX) $(CXX_FLAG) $(BOOST_INCLUDE) -o $(NAME) $< $(LDFLAGS)