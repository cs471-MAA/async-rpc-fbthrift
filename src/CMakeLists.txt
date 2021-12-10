# Copyright (c) Facebook, Inc. and its affiliates.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

option(DEBUG_PRINT "Message will be printed out in the console when using M_COUT, M_CERR, M_DEBUG_OUT, M_DEBUG_ERR" ON)
if(DEBUG_PRINT)
  add_definitions(-DDEBUG_PRINT)
  message(STATUS
        "The compiler will enable execution of M_COUT and M_CERR to print out in the console.")
endif()

option(DEBUG_LOG "Message will be logged out via glog when using M_LOGINFO, M_LOGERR, M_DEBUG_OUT, M_DEBUG_ERR" ON)
if(DEBUG_LOG)
  add_definitions(-DDEBUG_LOG)
  message(STATUS
        "The compiler will enable execution of M_LOGINFO and M_LOGERROR to log out in the glog.")
endif()

option(SYNC "Will tend to create a code for a synchronous execution." OFF)
if(SYNC)
  add_definitions(-DSYNC)
  message(STATUS
        "The code will be compiled with the SYNC flags")
else()
  add_definitions(-DASYNC)
  message(STATUS
        "The code will be compiled with the ASYNC flags")
endif()

option(LOCALHOST "Will create a code for localhost execution." OFF)
if(LOCALHOST)
  add_definitions(-DLOCALHOST)
  message(STATUS
        "The code will be compiled for localhost.")
else()
  message(STATUS
        "The code will be compiled for docker.")
endif()

option(OPTI3 "Will create a optimized code level 3." OFF)
if(OPTI3)
  add_definitions(-O3)
  message(STATUS
        "The code will be compiled with optimization level 3")
endif()


set(CMAKE_CXX_STANDARD 17)
PROJECT(ThriftTransport_CPP2_Example CXX)
# set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.16)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake_module")

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BUILD_DIR})

# fbThrift related dependencies
find_package(gflags REQUIRED)
#find_package(proxygen REQUIRED)
find_package(FBThrift REQUIRED)
find_package(Folly REQUIRED)
find_package(Wangle REQUIRED)
find_package(fizz REQUIRED)
find_package(Threads REQUIRED)

# facebook's simple service performance query lib
find_package(fb303 REQUIRED)
include_directories(${FB303_INCLUDE_DIR})

find_library(atomic-lib atomic)

# .thrift files. TODO: remove this file
# include(dep/if/CMakeLists.txt)

# .thrift file libraries
include(FBThriftCppLibrary)

add_fbthrift_cpp_library(
  fb303_cpp2
  common/fb303/if/fb303.thrift
  SERVICES
    FacebookService
)

# TODO: these two lines might not be necessary
#set(THRIFT1 "/usr/local/bin/thrift1")
#set(THRIFTCPP2 "/usr/local/lib/libthriftcpp2.a")

add_fbthrift_cpp_library(
  mock_message_board-cpp2
  dep/if/mock_message_board.thrift
  SERVICES
    MockDatabase
    MessageService
    SanitizationService
  DEPENDS
    fb303_cpp2
)

include_directories(dep)
include_directories(shared)

add_subdirectory(shared)
add_subdirectory(MessageService)
add_subdirectory(MockDatabase)
add_subdirectory(SanitizationService)
add_subdirectory(Client)

