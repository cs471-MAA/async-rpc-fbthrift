//
// Created by adrien on 11.11.21.
//

#include <Utils.h>
#include "MessageService.h"

void mock_message_board::MessageServiceHandler::find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id) {
    folly::EventBase eb;
    std::unique_ptr<MockDatabaseAsyncClient> client = newMockDatabaseRocketClient(&eb, addr1);
    client->sync_find_last_message(result, *client_id);
}

bool mock_message_board::MessageServiceHandler::send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) {
    bool ret_val;
    folly::EventBase eb;
    std::unique_ptr<newSanitizationServiceRocketClient> client = newSanitizationServiceRocketClient(&eb, addr2);
    ret_val = client->sync_sanitize_message(*client_id, *message);

    return ret_val;
}

mock_message_board::MessageServiceHandler::MessageServiceHandler() : clientLoopThread_(new folly::ScopedEventBaseThread())  {
    addr1 = folly::SocketAddress("127.0.0.1", 10001, true); // mock database
    addr2 = folly::SocketAddress("127.0.0.1", 10003, true); // sanitization
}

mock_message_board::MessageServiceHandler::~MessageServiceHandler() {
    delete clientLoopThread_;
}
