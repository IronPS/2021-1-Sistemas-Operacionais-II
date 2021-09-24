
cmake_minimum_required(VERSION 3.17)

project(ServerApp-ProducerConsumerBuffer VERSION 0.0.0)

set(SOURCES
    src/ProducerConsumerBuffer.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories( ${PROJECT_NAME}
    PUBLIC include
	PRIVATE "${PROJECT_BINARY_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC Common-Semaphore
    PUBLIC Common-PacketBuilder
)

