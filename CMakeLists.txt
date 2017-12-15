cmake_minimum_required(VERSION 3.2.2)
project(UniServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp ServerEngine.cpp ServerEngine.h MySqlAgent.cpp MySqlAgent.h MqttClient.cpp MqttClient.h HttpServer.cpp HttpServer.h GlobalDefs.h ConfigReader.cpp ConfigReader.h MongoAgent.cpp MongoAgent.h DeviceNode.cpp DeviceNode.h)
add_executable(UniServer ${SOURCE_FILES})

