
cmake_minimum_required(VERSION 3.17)

project(ClientApp VERSION 0.0.0)

set(SOURCES
    src/main.cpp
    src/parser.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories( ${PROJECT_NAME}
    PRIVATE include
	PRIVATE "${PROJECT_BINARY_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE Common-Stoppable
    PRIVATE ClientApp-ConnectionManager
    PRIVATE ServerApp-ReplicaManager
    PRIVATE ClientApp-CommandExecutor
    PRIVATE ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJ_INSTALL_DIR})

