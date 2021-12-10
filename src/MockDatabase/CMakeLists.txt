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

# set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(
	MockDatabase

	MockDatabase.cpp
	../shared/Utils.cpp
	../shared/ServerStats.cpp
)
target_link_libraries(
	MockDatabase

	mock_message_board-cpp2
	atomic
	FBThrift::thriftcpp2
	fb303
)

# add_executable(
# 	MockDatabaseMiddle

# 	MockDatabaseMiddle.cpp
# )
# target_link_libraries(
# 	MockDatabaseMiddle

# 	mock_message_board-cpp2
# 	atomic
# 	FBThrift::thriftcpp2
# )

add_executable(
	MockDatabaseClient

	MockDatabaseClient.cpp
	../shared/Utils.cpp
	../shared/ServerStats.cpp
)
target_link_libraries(
	MockDatabaseClient

	mock_message_board-cpp2
	atomic
	FBThrift::thriftcpp2
	fb303
)

# install(TARGETS server DESTINATION /usr/bin/)