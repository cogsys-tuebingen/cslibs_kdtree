cmake_minimum_required(VERSION 2.8.3)
project(kdtree)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED)

## Enforce that we use C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

catkin_package(
    INCLUDE_DIRS include
    #  LIBRARIES kd-tree
    #  CATKIN_DEPENDS other_catkin_pkg
    #  DEPENDS system_lib
    )

include_directories(include)


add_executable(kd-tree-test
    include/kd-tree/kdtree_cluster_mask.hpp
    include/kd-tree/kdtree_clustering.hpp
    include/kd-tree/kdtree.hpp
    include/kd-tree/buffered_kdtree.hpp
    include/kd-tree/buffered_kdtree_clustering.hpp
    include/kd-tree/kdtree_dotty.hpp
    src/kd-tree-test.cpp
)

