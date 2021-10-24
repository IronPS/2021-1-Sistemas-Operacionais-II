
cmake_minimum_required(VERSION 3.17)

project(ServerApp-SessionMonitor VERSION 0.0.0)

set(SOURCES
    src/SessionMonitor.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories( ${PROJECT_NAME}
    PUBLIC include
	PRIVATE "${PROJECT_BINARY_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC ServerApp-SessionController
    PUBLIC ServerApp-PersistenceManager
    PUBLIC ServerApp-MessageManager
    PUBLIC Common-CriticalSessionControl
)
