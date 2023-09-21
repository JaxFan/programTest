cmake_minimum_required(VERSION 3.10)

project(Test)

set(CMAKE_CXX_COMPILER "c++")
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS -g -Wall)

message(STATUS "CMAKE_CXX_FLAGS: " "${CMAKE_CXX_FLAGS}")

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: " "${CMAKE_CXX_FLAGS}")
#debug 用
 SET(CMAKE_BUILD_TYPE "Debug")  
 SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
 SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall") 

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/inc/
    ${PROJECT_SOURCE_DIR}/inc/multiProcessTest/
)

set(TSP_APP_SRCS
    src/main.c
    src/task/taskCall.c
    src/timer/timerCall.c
)

set(TEST_PROCESS_TEST
    src/multiProcessTest/ProcessTestMain.c
)



add_executable(ProgramTest ${TSP_APP_SRCS})
target_link_libraries(ProgramTest pthread)#pthread是保障ptherad semaphore等能正常使用的关键！！
install(TARGETS ProgramTest DESTINATION ${PROJECT_INSTALL_PREFIX}  COMPONENT Test)

add_executable(ProcessTest ${TEST_PROCESS_TEST})
target_link_libraries(ProcessTest pthread)#pthread是保障ptherad semaphore等能正常使用的关键！！
install(TARGETS ProcessTest DESTINATION ${PROJECT_INSTALL_PREFIX}  COMPONENT ProcessTest)