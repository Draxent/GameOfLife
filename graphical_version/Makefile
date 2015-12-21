# ---------------------------------
# Author: Federico Conte (Draxent)
# ---------------------------------

CXX			=	g++ -std=c++11
CXX_FLAGS  	=	
OPT_FLAGS	=	-O3
OPENCV_FLAG	=	-D OPENCV
LIBS		=	-lopencv_core -lX11 -lpthread -lopencv_imgproc -lopencv_highgui

# project name
TARGET   	=	game_of_life

# Directories
INCLUDE_DIR	=	include
SOURCE_DIR	=	src
BUILD_DIR	=	build

# Files
INCLUDES	=	$(wildcard $(INCLUDE_DIR)/*.hpp)
SOURCES		=	$(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS		=	$(INCLUDES:$(INCLUDE_DIR)/%.hpp=$(BUILD_DIR)/%.o)

.PHONY: all clean cleanall

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE_DIR) $(OPT_FLAGS) $(OPENCV_FLAG) $(SOURCE_DIR)/main.cpp $(OBJECTS) -o $@ $(LIBS)

$(OBJECTS): $(BUILD_DIR)/%.o : $(SOURCE_DIR)/%.cpp $(INCLUDE_DIR)/%.hpp
	@$(CXX) $(CXX_FLAGS) -I $(INCLUDE_DIR) -c $< -o $@ $(LIBS)
	@echo "Compiled "$<" successfully!"

clean:
	rm -f $(BUILD_DIR)/$(TARGET)
	@echo "Cleanup complete!"

cleanall:
	rm -f $(BUILD_DIR)/*~ $(BUILD_DIR)/*.o $(BUILD_DIR)/$(TARGET)
	@echo "Cleanup complete!"
