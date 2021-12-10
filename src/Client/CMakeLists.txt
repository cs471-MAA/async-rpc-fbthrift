
add_executable(
	client

	client.cpp
	../shared/Utils.cpp
	../shared/ServerStats.cpp
)
target_link_libraries(
	client

	mock_message_board-cpp2
	atomic
	FBThrift::thriftcpp2
	fb303
)