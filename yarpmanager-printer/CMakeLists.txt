# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.
 
# YARP needs CMake 3.0 or greater
cmake_minimum_required(VERSION 3.12)
 
# project name
project(yarpmanager-printer
VERSION 1.0.0
LANGUAGES CXX)

# find YARP
find_package(YARP)
find_package(YARP COMPONENTS os REQUIRED)
find_package(YCM 0.11 REQUIRED)
find_package(YARP COMPONENTS os manager dev REQUIRED)

# set up our program
add_executable(yarpmanager-printer)
 
# declare our source files
target_sources(yarpmanager-printer PRIVATE yarpmanager-printer.cpp)
 
# link with YARP libraries                                    
target_link_libraries(yarpmanager-printer PRIVATE 
                                YARP::YARP_os
                                YARP::YARP_init 
                                YARP::YARP_manager)
                    
