cmake_minimum_required(VERSION 2.8.4)
project(MessageThrottler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lpthread -lCppConfigManager -lxerces-c -llog4cpp")
link_directories("$ENV{HOME}/git/MessageThrottler")

# Add Executable Binary below.
add_executable(MessageThrottler Application.cpp  Logger.cpp  Main.cpp  Order.cpp  OrderValidator.cpp)

