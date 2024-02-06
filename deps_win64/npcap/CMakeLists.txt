cmake_minimum_required(VERSION 3.23)
project(ethernet_toolbox)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
link_directories(lib)

add_executable(ethernet_toolbox src/main.cpp)
target_link_libraries(ethernet_toolbox
        Packet.lib
        wpcap.lib)
