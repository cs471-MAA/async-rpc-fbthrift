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

#message(FATAL_ERROR ${CMAKE_CURRENT_SOURCE_DIR})
set(THRIFT1 "/usr/local/bin/thrift1")
set(THRIFTCPP2 "/usr/local/lib/libthriftcpp2.a")

set(EXAMPLE_IF_HOME ${CMAKE_CURRENT_SOURCE_DIR}/dep/if)
include(/usr/local/include/thrift/ThriftLibrary.cmake)
# Example to test linking
thrift_library(
  "chatroom" #file_name
  "ChatRoomService;Echo" #services
  "cpp2" #language
  "" #options
  "${EXAMPLE_IF_HOME}" #file_path
  "${EXAMPLE_IF_HOME}" #output_path
  "if" #include_prefix
)
