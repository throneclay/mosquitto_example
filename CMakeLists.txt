cmake_minimum_required(VERSION 2.8)
PROJECT (mosquitto_example)
 
# Find required protobuf package, if your find_package protobuf not work, try uncomment these codes
#set(protobuf_root "/home/zhangshuai/miniconda3/envs/pp_py36")
#set(Protobuf_INCLUDE_DIR "${protobuf_root}/include")
#set(Protobuf_LIBRARY "${protobuf_root}/lib/libprotobuf.so")

find_package(Protobuf REQUIRED)
if(PROTOBUF_FOUND)
    message(STATUS "protobuf library found")
else()
    message(FATAL_ERROR "protobuf library is needed but cant be found")
endif()

# Find mosquitto 
set(MOSQUITTO_ROOT "scripts/mosquitto/usr/local")
include(cmake/findmosquitto.cmake)
find_mosquitto()

include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${MOSQUITTO_INCLUDE_DIR})

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS proto/Message.proto)

ADD_EXECUTABLE(sub_callback src/sub_callback.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(sub_callback ${PROTOBUF_LIBRARIES} ${MOSQUITTO_LIBRARIES})

ADD_EXECUTABLE(pub_simple src/pub_simple.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(pub_simple ${PROTOBUF_LIBRARIES} ${MOSQUITTO_LIBRARIES})

