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


void mock_message_board::MessageServiceHandler::find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id, int64_t query_uid) {
    manager.add_entry(query_uid, get_epoch_time_us());
    // fb303 counter
    // const auto p1 = std::chrono::system_clock::now();
    // fb303::fbData->setCounter(
    //     "find_last_message.date",
    //     (int64_t) std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count()
    // );

    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "find_last_message: client_id=" << *client_id << " query_uid=" << query_uid);
    std::string result2 = result;
    std::string clientIDL = *client_id;

    extCo->sync_find_last_message(result2, clientIDL, query_uid);
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "sync_find_last_message: query_uid=" << query_uid << ": received!");

    result = result2;
    
    manager.add_entry(query_uid, get_epoch_time_us());
}

bool mock_message_board::MessageServiceHandler::send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message, int64_t query_uid) {
    manager.add_entry(query_uid, get_epoch_time_us());
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "send_message: client_id=" << *client_id << " query_uid=" << query_uid << " | message=" << *message);

    bool res;

    std::string clientIDL = *client_id;
    std::string messageL = *message;

    res = extCo->sync_sanitize_message(clientIDL, messageL, query_uid);
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "sync_store_message " << query_uid << ": received!");


    manager.add_entry(query_uid, get_epoch_time_us());
    return res;
}

