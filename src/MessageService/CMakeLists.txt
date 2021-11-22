
add_executable(
		MessageServiceServer

		MessageServiceServer.cpp
		MessageService.cpp
		../shared/Utils.cpp
)
target_link_libraries(
		MessageServiceServer

PUBLIC
		mock_message_board-cpp2
		atomic
		FBThrift::thriftcpp2
		fb303
)
