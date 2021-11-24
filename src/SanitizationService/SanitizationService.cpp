//
// Created by adrien on 11.11.21.
//

#include <Utils.h>
#include <regex>
#include <dep/if/gen-cpp2/MessageService.h>
#include "SanitizationService.h"

using mock_message_board::MessageServiceAsyncClient;

bool mock_message_board::SanitizationHandler::sanitize_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) {
    std::cout << "sanitization|sanitize_message: received client_id=" << *client_id << " | message=" << *message << std::endl;

    std::regex match_expr("^.*[\\/*$^].*$");
    if (std::regex_match(*message, match_expr)) {
        return false;
    }

    folly::EventBase eb;
    auto client = newRocketClient<MockDatabaseAsyncClient>(&eb, this->addr);
    client->sync_store_message(*client_id, *message);

    return true;
}

mock_message_board::SanitizationHandler::SanitizationHandler() : clientLoopThread_(new folly::ScopedEventBaseThread())  {
    #ifdef LOCALHOST
        addr = folly::SocketAddress("127.0.0.1", 10001, true); // mock database
    #else
        addr = folly::SocketAddress("mock-database", 10001, true); // mock database
    #endif
}

mock_message_board::SanitizationHandler::~SanitizationHandler() {
    delete clientLoopThread_;
}
