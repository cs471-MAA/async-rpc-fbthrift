
add_definitions(-DLOCALHOST)
add_executable(
	client

	client.cpp
)
target_link_libraries(
	client

	mock_message_board-cpp2
	atomic
	FBThrift::thriftcpp2
)