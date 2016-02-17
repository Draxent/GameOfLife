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
FASTFLOW = false
BOOST = false
MIC = false
DEBUG = false
NAME = $@
VECT = true
INFO_VECT = false

ifeq ($(VECT),true)
	ifeq ($(INFO_VECT),true)
		VECTORIZATION = -opt-report-phase:vec -opt-report-file=stderr -qopt-report=5
	endif
else
	VECTORIZATION = -no-vec
endif

ifeq ($(BOOST),true)
	BOOST_INCLUDE = -I $(BOOST_ROOT) -L $(BOOST_ROOT)/stage/lib
endif

ifeq ($(MIC),true)
	XEONPHI = -mmic -DNO_DEFAULT_MAPPING
endif

ifeq ($(DEBUG),true)
	OPTFLAGS = -g -D DEBUG
else
        OPTFLAGS = -O3 -finline-functions -DNDEBUG
endif

CXX	= icpc $(XEONPHI) -std=c++11 $(VECTORIZATION)
LDFLAGS = -pthread

.SUFFIXES: .cpp 

$(TARGET): $(TARGET).cpp
	$(CXX) $(BOOST_INCLUDE) $(OPTFLAGS) -o $(NAME) $< $(LDFLAGS)