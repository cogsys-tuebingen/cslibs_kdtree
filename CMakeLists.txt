cmake_minimum_required(VERSION 2.8.3)
project(cslibs_kdtree)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED)

## Enforce that we use C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    add_definitions(-std=c++11)
elseif(COMPILER_SUPPORTS_CXX0X)
    add_definitions(-std=c++0x)
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

add_executable(kdtree-test
    src/kdtree-test.cpp
)

add_custom_target(show_kdtree_headers_in_qt SOURCES
    include/cslibs_kdtree/kdtree_clustering.hpp
    include/cslibs_kdtree/kdtree_node_neighbourhood.hpp
    include/cslibs_kdtree/kdtree_node.hpp
    include/cslibs_kdtree/kdtree_unbuffered.hpp
    include/cslibs_kdtree/kdtree_buffered.hpp
    include/cslibs_kdtree/kdtree_dotty.hpp
    include/cslibs_kdtree/kdtree.hpp
    include/cslibs_kdtree/array.hpp
    include/cslibs_kdtree/index.hpp
    include/cslibs_kdtree/page.hpp
    include/cslibs_kdtree/page_clustering.hpp
    include/cslibs_kdtree/array_clustering.hpp
    include/cslibs_kdtree/fill.hpp
)

install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
