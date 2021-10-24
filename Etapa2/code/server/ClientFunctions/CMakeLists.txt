
cmake_minimum_required(VERSION 3.17)

project(ServerApp-ClientFunctions VERSION 0.0.0)

set(SOURCES
    src/ClientFunctions.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories( ${PROJECT_NAME}
    PUBLIC include
	PRIVATE "${PROJECT_BINARY_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE ServerApp-ConnectionManager
    PRIVATE ServerApp-ReplicaManager
    PRIVATE ServerApp-PersistenceManager
    PRIVATE ServerApp-SessionMonitor
    PRIVATE Common-Stoppable
    PRIVATE ${CMAKE_THREAD_LIBS_INIT}
)

