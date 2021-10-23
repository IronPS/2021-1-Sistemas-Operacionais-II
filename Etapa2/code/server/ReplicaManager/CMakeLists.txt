
cmake_minimum_required(VERSION 3.17)

project(ServerApp-ReplicaManager VERSION 0.0.0)

set(SOURCES
    src/ReplicaManager.cpp
    src/ReplicaConnection.cpp
    src/ElectionManager.cpp
    src/ReplicationManager.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories( ${PROJECT_NAME}
    PUBLIC include
	PRIVATE "${PROJECT_BINARY_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC ServerApp-ConnectionManager
    PUBLIC ClientApp-ConnectionManager
    PUBLIC Common-Stoppable
    PUBLIC Common-CriticalSessionControl
    PUBLIC Common-PacketBuilder
    PUBLIC ServerApp-PersistenceManager
    PUBLIC ServerApp-SessionMonitor
    PRIVATE ${CMAKE_THREAD_LIBS_INIT}
)
