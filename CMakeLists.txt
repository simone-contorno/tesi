# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.
 
# YARP needs CMake 3.0 or greater
cmake_minimum_required(VERSION 3.12)
 
# project name
project(printer2)

# find YARP
#find_package(YARP COMPONENTS os REQUIRED)
#find_package(YARP COMPONENTS sig REQUIRED)
#find_package(YARP COMPONENTS math REQUIRED)
#find_package(YARP COMPONENTS manager REQUIRED)
#find_package(YARP COMPONENTS manager REQUIRED)
find_package(YARP)
 
# set up our program
add_executable(printer2)
 
# declare our source files
target_sources(printer2 PRIVATE printer2.cpp)
 
# link with YARP libraries
target_link_libraries(printer2 PRIVATE YARP::YARP_os
                                    YARP::YARP_init)
                                    
target_link_libraries(printer2 PRIVATE YARP::YARP_os
                                    YARP::YARP_init YARP::YARP_manager)
