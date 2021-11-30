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

uint32_t MSGSERV_MOCK_TIMEOUT = 20000;
uint32_t MSGSERV_SANIT_TIMEOUT = 30000;

void mock_message_board::MessageServiceHandler::find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id) {
    // fb303 counter
    const auto p1 = std::chrono::system_clock::now();
    fb303::fbData->setCounter(
        "find_last_message.date",
        (int64_t) std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count()
    );

    std::cout << "message-service | find_last_message: client_id=" << *client_id << std::endl;

    auto search = dbMap.find(std::this_thread::get_id());
    if(search == dbMap.end()){
        auto *eb = new folly::EventBase();
        cout << "\tNew client for thread ID " << std::this_thread::get_id() << ": sending..."  << "\n";

        dbMap.insert({std::this_thread::get_id(), newRocketClient<MockDatabaseAsyncClient>(eb, addr1, MSGSERV_MOCK_TIMEOUT)})
                .first->second->sync_find_last_message(result, *client_id);
    }else{
        cout << "\tThread ID " << std::this_thread::get_id() << ": sending..."  << "\n";
        search->second->sync_find_last_message(result, *client_id);
    }

    cout << "\tThread ID " << std::this_thread::get_id() << ": received!" << "\n";
}

bool mock_message_board::MessageServiceHandler::send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) {
    std::cout << "message-service | send_message: client_id=" << *client_id << " | message=" << *message << std::endl;

    auto search = sanMap.find(std::this_thread::get_id());
    bool res = false;
    if(search == sanMap.end()){
        auto *eb = new folly::EventBase();
        cout << "\tNew client for thread ID " << std::this_thread::get_id() << ": sending..."  << "\n";

        res = sanMap.insert({std::this_thread::get_id(), newRocketClient<SanitizationServiceAsyncClient>(eb, addr2, MSGSERV_SANIT_TIMEOUT)})
        .first->second->sync_sanitize_message(*client_id, *message);
    }else{
        cout << "\tThread ID " << std::this_thread::get_id() << ": sending..."  << "\n";
        res = search->second->sync_sanitize_message(*client_id, *message);
    }
    cout << "\tThread ID " << std::this_thread::get_id() << ": received!" << "\n";
    return res;
}
