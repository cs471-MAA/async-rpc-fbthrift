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

add_executable(
		server

		server.cpp
)
target_link_libraries(
		server

		example-cpp2
		atomic
		FBThrift::thriftcpp2
)
install(TARGETS server DESTINATION /usr/bin/)

add_executable(
		client

		client.cpp
)
target_link_libraries(
		client

		example-cpp2
		atomic
		FBThrift::thriftcpp2
)
install(TARGETS client DESTINATION /usr/bin/)
