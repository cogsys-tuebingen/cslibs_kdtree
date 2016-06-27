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
    src/kd-tree-test.cpp

)

add_executable(kdtree-buffered-test
    src/kdtree-buffered-test.cpp
)

add_custom_target(show_kdtree_headers_in_qt SOURCES
    include/kdtree/kdtree_cluster_mask.hpp
    include/kdtree/kdtree_clustering.hpp
    include/kdtree/kdtree_node.hpp
    include/kdtree/kdtree.hpp
    include/kdtree/buffered_kdtree_clustering.hpp
    include/kdtree/buffered_kdtree_node.hpp
    include/kdtree/buffered_kdtree.hpp
    include/kdtree/kdtree_dotty.hpp

    include/kdtree/buffered/kdtree_clustering.hpp
    include/kdtree/buffered/kdtree_node_neighbourhood.hpp
    include/kdtree/buffered/kdtree_node.hpp
    include/kdtree/buffered/kdtree.hpp
)

