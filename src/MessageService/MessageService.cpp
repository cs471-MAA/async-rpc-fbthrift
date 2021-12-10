//
// Created by adrien on 11.11.21.
//

#include <Utils.h>
#include <dep/if/gen-cpp2/MockDatabase.h>
#include <dep/if/gen-cpp2/SanitizationService.h>
#include <fb303/ServiceData.h>
#include <iostream>
#include <chrono>
#include <thrift/lib/cpp2/GeneratedCodeHelper.h>
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

    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "find_last_message: client_id=" << *client_id);

    std::string result2;
    eb.runInEventBaseThread([&]() {
        dbClient->sync_find_last_message(result, *client_id);
        M_DEBUG_OUT("\tThread ID " << std::this_thread::get_id() << ": received!");
    });
}

bool mock_message_board::MessageServiceHandler::send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) {
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "send_message: client_id=" << *client_id << " | message=" << *message);

    bool result2;
    eb.runInEventBaseThread([&]() {
        result2 = dbClient->sync_store_message(*client_id, *message);
        M_DEBUG_OUT("\tThread ID " << std::this_thread::get_id() << ": received!");
    });
    return result2;
}