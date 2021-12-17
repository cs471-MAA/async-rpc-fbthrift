
add_executable(
	Client

	Client.cpp
	../shared/Utils.cpp
	../shared/ServerStats.cpp
)
target_link_libraries(
	Client

	mock_message_board-cpp2
	atomic
	FBThrift::thriftcpp2
	fb303
)