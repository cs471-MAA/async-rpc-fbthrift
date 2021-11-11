//
// Created by adrien on 11.11.21.
//

#include <Utils.h>
#include "MessageService.h"

void mock_message_board::MessageServiceHandler::find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id) {
    folly::EventBase eb;
    std::unique_ptr<MockDatabaseAsyncClient> client = newMockDatabaseRocketClient(&eb, addr);
    client->sync_find_last_message(result, *client_id);

}

bool mock_message_board::MessageServiceHandler::send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) {
    bool ret_val;
    folly::EventBase eb;
    std::unique_ptr<MockDatabaseAsyncClient> client = newMockDatabaseRocketClient(&eb, addr);
    ret_val = client->sync_store_message(*client_id, *message);

    return ret_val;
}

mock_message_board::MessageServiceHandler::MessageServiceHandler() : clientLoopThread_(new folly::ScopedEventBaseThread())  {
    #ifdef LOCALHOST
        addr = folly::SocketAddress("127.0.0.1", 10001, true);
    #else 
        addr = folly::SocketAddress("mock-database", 10001, true);
    #endif
}

mock_message_board::MessageServiceHandler::~MessageServiceHandler() {
    delete clientLoopThread_;
}
