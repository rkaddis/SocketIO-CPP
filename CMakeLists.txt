cmake_minimum_required(VERSION 3.10) 

project(SocketIO)


set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target")

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PAHO_MQTT_C_INCLUDE_DIRS})

# Add an executable target
add_executable(subscriber src/subscriber.cpp)
add_executable(publisher src/publisher.cpp) 


target_link_libraries(subscriber paho-mqtt3a paho-mqttpp3) 
target_link_libraries(publisher paho-mqtt3a paho-mqttpp3) 