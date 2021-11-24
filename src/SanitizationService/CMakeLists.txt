add_definitions(-DLOCALHOST)

add_executable(
    SanitizationServiceServer

    SanitizationService.cpp
    SanitizationServiceServer.cpp
    # ${THRIFT_GEN_CPP_DIR}/SanitizationService.cpp
    # ${THRIFT_GEN_CPP_DIR}/MessageService.cpp
    ../shared/Utils.cpp
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