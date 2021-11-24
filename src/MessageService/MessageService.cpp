//
// Created by adrien on 11.11.21.
//

#include <Utils.h>
#include <dep/if/gen-cpp2/MockDatabase.h>
#include <dep/if/gen-cpp2/SanitizationService.h>
#include <fb303/ServiceData.h>
#include <iostream>
#include <chrono>
#include "MessageService.h"

namespace fb303 = facebook::fb303;

using mock_message_board::MockDatabaseAsyncClient;
using mock_message_board::SanitizationServiceAsyncClient;


void mock_message_board::MessageServiceHandler::find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id) {
    // fb303 counter
    const auto p1 = std::chrono::system_clock::now();
    fb303::fbData->setCounter(
        "find_last_message.date",
        (int64_t) std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count()
    );

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