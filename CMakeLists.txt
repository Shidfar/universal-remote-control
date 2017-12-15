<<<<<<< HEAD
cmake_minimum_required(VERSION 3.2.2)
project(UniServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp ServerEngine.cpp ServerEngine.h MySqlAgent.cpp MySqlAgent.h MqttClient.cpp MqttClient.h HttpServer.cpp HttpServer.h GlobalDefs.h ConfigReader.cpp ConfigReader.h MongoAgent.cpp MongoAgent.h DeviceNode.cpp DeviceNode.h)
add_executable(UniServer ${SOURCE_FILES})
=======
cmake_minimum_required(VERSION 3.6)
project(UniR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    UniversalRemote.cpp)

add_executable(UniR ${SOURCE_FILES})
>>>>>>> f3e240460e57aad45b97de6400e4b5bb521997f5
