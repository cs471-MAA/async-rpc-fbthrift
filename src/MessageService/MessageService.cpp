//
// Created by adrien on 11.11.21.
//

#include <Utils.h>
#include <cstdint>
#include <dep/if/gen-cpp2/MockDatabase.h>
#include <dep/if/gen-cpp2/SanitizationService.h>
#include <fb303/ServiceData.h>
#include <iostream>
#include <chrono>
#include "MessageService.h"

using mock_message_board::MockDatabaseAsyncClient;
using mock_message_board::SanitizationServiceAsyncClient;

uint32_t MSGSERV_MOCK_TIMEOUT = 20000;
uint32_t MSGSERV_SANIT_TIMEOUT = 30000;

void mock_message_board::MessageServiceHandler::find_last_message(MessageResponse& resp, std::unique_ptr<::std::string> client_id, int64_t query_uid) {
    // log and stats
    manager.add_entry(query_uid, get_epoch_time_us());
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "find_last_message: client_id=" << *client_id);

    auto search = dbMap.find(std::this_thread::get_id());
    ::std::string message;
    
    if(search == dbMap.end()){
        auto *eb = new folly::EventBase();
        M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "New client for thread ID " << std::this_thread::get_id() << ": sending query: " << (uint64_t)query_uid);

        dbMap.insert({std::this_thread::get_id(), newRocketClient<MockDatabaseAsyncClient>(eb, addr1, MSGSERV_MOCK_TIMEOUT)})
                .first->second->sync_find_last_message(message, *client_id, query_uid);
    }else{
        M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "Thread ID " << std::this_thread::get_id() << ": sending query: " << (uint64_t)query_uid);
        search->second->sync_find_last_message(message, *client_id, query_uid);
    }

    // response
    resp.query_uid_ref() = query_uid;
    resp.message_ref() = message;

    // log and stats
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "Thread ID " << std::this_thread::get_id() << ": received query: " << (uint64_t)query_uid);
    manager.add_entry(query_uid, get_epoch_time_us());
}

void mock_message_board::MessageServiceHandler::send_message(StatusResponse& resp, std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message, int64_t query_uid) {
    // log and stats
    manager.add_entry(query_uid, get_epoch_time_us());
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "send_message: client_id=" << *client_id << " | message=" << *message);

    auto search = sanMap.find(std::this_thread::get_id());
    bool res = false;
    if(search == sanMap.end()){
        auto *eb = new folly::EventBase();
        M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "New client for thread ID " << std::this_thread::get_id() << ": sending query: " << query_uid);

        res = sanMap.insert({std::this_thread::get_id(), newRocketClient<SanitizationServiceAsyncClient>(eb, addr2, MSGSERV_SANIT_TIMEOUT)})
        .first->second->sync_sanitize_message(*client_id, *message, query_uid);
    }else{
        M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "Thread ID " << std::this_thread::get_id() << ": sending query: " << (uint64_t)query_uid);
        res = search->second->sync_sanitize_message(*client_id, *message, query_uid);
    }

    // response
    resp.ok_ref() = res;
    resp.query_uid_ref() = query_uid;

    // log and stats
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "Thread ID " << std::this_thread::get_id() << ": received query: " << (uint64_t)query_uid);
    manager.add_entry(query_uid, get_epoch_time_us());
}
