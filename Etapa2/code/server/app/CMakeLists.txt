
cmake_minimum_required(VERSION 3.17)

project(ServerApp VERSION 0.0.0)

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
    PRIVATE ServerApp-Server
    PRIVATE ServerApp-ConnectionManager
    PRIVATE ServerApp-Server
    PRIVATE ServerApp-PersistenceManager
    PRIVATE ServerApp-SessionMonitor
    PRIVATE Common-Stoppable
    PRIVATE ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJ_INSTALL_DIR})

