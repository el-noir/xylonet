cmake_minimum_required(VERSION 3.10)
project(Xylonet)

set(CMAKE_CXX_STANDARD 14)

add_executable(Xylonet DAG.cpp TransactionNode.cpp HashUtils.cpp Xylonet.cpp)
