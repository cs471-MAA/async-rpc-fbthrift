
add_executable(
		MessageService

		MessageServiceServer.cpp
)
target_link_libraries(
		MessageService

		example-cpp2
		atomic
		FBThrift::thriftcpp2
)
