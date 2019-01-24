cmake_minimum_required(VERSION 2.8.4)
project(louvain-community)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++03")

include_directories(./)

add_executable(community
        main_community.cpp
        graph_binary.h
        community.h
        community.cpp
        graph_binary.cpp
        graph.cpp
        graph.h
        )

add_executable(convert
        main_convert.cpp
        graph.h
        graph.cpp
        )

add_executable(hierarchy
        main_hierarchy.cpp
        )