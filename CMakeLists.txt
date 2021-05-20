# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.
 
# YARP needs CMake 3.0 or greater
cmake_minimum_required(VERSION 3.12)
 
# Project name
project(yarpmanager-console2
            VERSION 1.0.0
            LANGUAGES CXX)

# To compile using threads
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pthread")
endif(CMAKE_COMPILER_IS_GNUCC)

# Find packages
find_package(YARP)
find_package(YARP COMPONENTS os REQUIRED)
find_package(YCM 0.11 REQUIRED)
find_package(YARP COMPONENTS os manager dev REQUIRED)
find_package(Threads REQUIRED)

# Set up program
add_executable(yarpmanager-console2)
 
# Declare source files
target_sources(yarpmanager-console2 PRIVATE yarpmanager-console2.cpp)
 
# Link with libraries                                    
target_link_libraries(yarpmanager-console2 PRIVATE YARP::YARP_os
                                                   YARP::YARP_init 
                                                   YARP::YARP_manager
                                                   Threads::Threads)
                    
