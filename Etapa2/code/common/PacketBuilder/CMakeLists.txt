
cmake_minimum_required(VERSION 3.17)

project(Common-PacketBuilder VERSION 0.0.0)

set(SOURCES
    src/PacketBuilder.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC include
	PRIVATE "${PROJECT_BINARY_DIR}/include"
)
