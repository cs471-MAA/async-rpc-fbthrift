add_executable(
		MessageServiceServer

		MessageServiceServer.cpp
		MessageService.cpp
		../shared/Utils.cpp
		../shared/ServerStats.cpp
        externalCo.cpp externalCo.h)
target_link_libraries(
		MessageServiceServer

PUBLIC
		mock_message_board-cpp2
		atomic
		FBThrift::thriftcpp2
		fb303
)
