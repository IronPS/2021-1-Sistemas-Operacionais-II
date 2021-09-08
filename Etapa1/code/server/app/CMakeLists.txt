
cmake_minimum_required(VERSION 3.10)

project(ServerApp VERSION 0.0.0)

set(SOURCES
    src/main.cpp
    src/parser.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories( ${PROJECT_NAME}
    PRIVATE
	    include
	    "${PROJECT_BINARY_DIR}/include"
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR})

# Not needed yet
# target_link_libraries(${PROJECT_NAME} 
# 	PRIVATE libraryname
# )
