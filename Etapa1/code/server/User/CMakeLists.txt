
cmake_minimum_required(VERSION 3.17)

project(ServerApp-User VERSION 0.0.0)

set(SOURCES
    src/User.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories( ${PROJECT_NAME}
    PUBLIC include
	PRIVATE "${PROJECT_BINARY_DIR}/include"
)

# Not needed yet
# target_link_libraries(${PROJECT_NAME} 
# 	PRIVATE libraryname
# )
