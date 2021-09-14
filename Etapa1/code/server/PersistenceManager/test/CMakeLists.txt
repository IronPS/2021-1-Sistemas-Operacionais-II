
cmake_minimum_required(VERSION 3.17)

project(ServerApp-PersistenceManager-tests VERSION 0.0.0)

include(GoogleTest)

add_executable(
    ${PROJECT_NAME}-BasicFunctionalityTests
    BasicFunctionalityTests.cpp
)
target_link_libraries(
    ${PROJECT_NAME}-BasicFunctionalityTests
    gtest_main
    ServerApp-PersistenceManager
)
gtest_discover_tests(${PROJECT_NAME}-BasicFunctionalityTests)


# Adding definitions and copying resources folder
add_definitions(-DRESOURCES="${PROJECT_BINARY_DIR}/resources/")
add_definitions(-DRESOURCES_TMP="${PROJECT_BINARY_DIR}/resources-tmp/")

add_custom_command(TARGET ${PROJECT_NAME}-BasicFunctionalityTests PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}-BasicFunctionalityTests>/resources
)
add_custom_command(TARGET ${PROJECT_NAME}-BasicFunctionalityTests PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}-BasicFunctionalityTests>/resources-tmp
)

