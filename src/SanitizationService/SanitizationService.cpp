//
// Created by adrien on 11.11.21.
//

#include <Utils.h>
#include <regex>
#include "SanitizationService.h"

bool mock_message_board::SanitizationHandler::sanitize_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) {
    std::regex match_expr("^.*[\\/*$^].*$");
    if (std::regex_match(*message, match_expr)) {
        return false;
    }

    folly::EventBase eb;
    std::unique_ptr<MockDatabaseAsyncClient> client = newMockDatabaseRocketClient(&eb, this->addr);
    client->sync_store_message(*client_id, *message);

    return true;
}

mock_message_board::SanitizationHandler::SanitizationHandler() : clientLoopThread_(new folly::ScopedEventBaseThread())  {
    addr = folly::SocketAddress("127.0.0.1", 10001, true); // mock database
}

mock_message_board::SanitizationHandler::~SanitizationHandler() {
    delete clientLoopThread_;
}
