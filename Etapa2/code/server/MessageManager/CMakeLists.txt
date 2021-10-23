
cmake_minimum_required(VERSION 3.17)

project(ServerApp-MessageManager VERSION 0.0.0)

set(SOURCES
    src/MessageManager.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories( ${PROJECT_NAME}
    PUBLIC include
	PRIVATE "${PROJECT_BINARY_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC Common-CriticalSessionControl
    PUBLIC Common-PacketBuilder
    PUBLIC ServerApp-ProducerConsumerBuffer
    PUBLIC ServerApp-User
)

