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
option(SYNC "Will tend to create a code for a synchronous execution." OFF)
if(SYNC)
  add_definitions(-DSYNC)
  message(STATUS
        "The code will be compiled with the SYNC flags")
endif()

option(LOCALHOST "Will create a code for localhost execution." OFF)
if(LOCALHOST)
  add_definitions(-DLOCALHOST)
  message(STATUS
        "The code will be compiled with localhost.")
endif()


set(CMAKE_CXX_STANDARD 17)
PROJECT(ThriftTransport_CPP2_Example CXX)
# set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.16)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake_module")

find_package(gflags REQUIRED)
#find_package(proxygen REQUIRED)
find_package(FBThrift REQUIRED)
find_package(Folly REQUIRED)
find_package(Wangle REQUIRED)
find_package(fizz REQUIRED)
find_package(Threads REQUIRED)

find_library(atomic-lib atomic)

include(dep/if/CMakeLists.txt)

include_directories(dep)
include_directories(shared)

add_subdirectory(MessageService)
add_subdirectory(MockDatabase)
add_subdirectory(SanitizationService)
add_subdirectory(Client)

