add_executable(
    SanitizationServiceServer

    SanitizationService.cpp
    SanitizationServiceServer.cpp
    ../shared/Utils.cpp
    ../shared/ServerStats.cpp
)

target_link_libraries(
    SanitizationServiceServer

    mock_message_board-cpp2
    FBThrift::thriftcpp2
)

# add_executable(
#     SanitizationServiceClient

#     SanitizationServiceClient.cpp
# )
# target_link_libraries(
#     SanitizationServiceClient

#     mock_message_board-cpp2
#     atomic
#     FBThrift::thriftcpp2
# )