SHELL			= /bin/bash

BIN				= IncredibleTvitter

SRC_FOLDER		= ./src
INC_FOLDER		= ./include
BUILD_FOLDER	= ./build

CXX				= g++
CXX_FLAGS		= -std=c++11 -g -Wall -I$(INC_FOLDER)

SRC_FILES		= $(wildcard $(SRC_FOLDER)/*.cpp)
OBJ_FILES		= $(patsubst $(SRC_FOLDER)/%.cpp, $(BUILD_FOLDER)/%.o, $(SRC_FILES))

.PHONY: all build clean

all: clean build_folder build

build: $(OBJ_FILES)
	$(CXX) $(CXX_FLAGS) -o $(BIN) $?

clean:
	echo $(OBJ_FILES)
	rm -rf ${BUILD_FOLDER}
	rm -f ${BIN}

build_folder:
	mkdir -p ${BUILD_FOLDER}

$(BUILD_FOLDER)/%.o: $(SRC_FOLDER)/%.cpp 
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

