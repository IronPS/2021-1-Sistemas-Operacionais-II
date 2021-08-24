SHELL           = /bin/bash

CLIENT_APP      = app_client
SERVER_APP      = app_server
EXECS           = $(CLIENT_APP) $(SERVER_APP)

SRC_FOLDER      = ./src
INC_FOLDER      = ./include
TP_INC_FOLDER   = ./include/third_party
BUILD_FOLDER    = ./build

CXX             = g++
CXX_FLAGS       = -std=c++11 -g -Wall -I$(INC_FOLDER) -I$(TP_INC_FOLDER)

SRC_FILES       = $(wildcard $(SRC_FOLDER)/*.cpp)
ALL_OBJ_FILES   = $(patsubst $(SRC_FOLDER)/%.cpp, $(BUILD_FOLDER)/%.o, $(SRC_FILES))
EXEC_OBJ_FILES  = $(addprefix $(BUILD_FOLDER)/, serverMain.o clientMain.o)
OBJ_FILES       = $(filter-out $(EXEC_OBJ_FILES), $(ALL_OBJ_FILES))

.PHONY: all build clean

all: clean build_folder build

build: build_server build_client

build_server: $(OBJ_FILES) $(BUILD_FOLDER)/serverMain.o
	$(CXX) $(CXX_FLAGS) -o $(SERVER_APP) $?

build_client: $(OBJ_FILES) $(BUILD_FOLDER)/clientMain.o
	$(CXX) $(CXX_FLAGS) -o $(CLIENT_APP) $?

clean:
	rm -rf ${BUILD_FOLDER}
	rm -f ${EXECS}

build_folder:
	mkdir -p ${BUILD_FOLDER}

$(BUILD_FOLDER)/%.o: $(SRC_FOLDER)/%.cpp 
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

