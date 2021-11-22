//
// Created by adrien on 11.11.21.
//

#include <Utils.h>
#include <if/gen-cpp2/MockDatabase.h>
#include <if/gen-cpp2/SanitizationService.h>
#include <iostream>
#include "MessageService.h"

using mock_message_board::MockDatabaseAsyncClient;
using mock_message_board::SanitizationServiceAsyncClient;


void mock_message_board::MessageServiceHandler::find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id) {
    std::map<std::string, int64_t> counters;
    this->getCounters(counters);
    for(auto item : counters) {
        std::cout << "counter: " << item.first << "=" << item.second << std::endl;
    }
    std::cout << "message-service|find_last_message: received client_id=" << *client_id << std::endl;
    
    folly::EventBase eb;
    auto client = newRocketClient<MockDatabaseAsyncClient>(&eb, addr1);
    client->sync_find_last_message(result, *client_id);
}

bool mock_message_board::MessageServiceHandler::send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) {
    std::cout << "message-service|send_message: received client_id=" << *client_id << " | message=" << *message << std::endl;
    
    bool ret_val;
    folly::EventBase eb;
    auto client = newRocketClient<SanitizationServiceAsyncClient>(&eb, addr2);
    ret_val = client->sync_sanitize_message(*client_id, *message);

    return ret_val;
}