//
// Created by adrien on 11.11.21.
//

#include <Utils.h>
#include <regex>
#include <if/gen-cpp2/MessageService.h>
#include "SanitizationService.h"

using mock_message_board::MessageServiceAsyncClient;

bool mock_message_board::SanitizationHandler::sanitize_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) {
    std::cout << "sanitization|sanitize_message: received client_id=" << *client_id << " | message=" << *message << std::endl;

    std::regex match_expr("^.*[\\/*$^].*$");
    if (std::regex_match(*message, match_expr)) {
        return false;
    }

    auto search = dbMap.find(std::this_thread::get_id());
    if(search == dbMap.end()){
        auto *eb = new folly::EventBase();
        cout << "created new client for thread ID " << std::this_thread::get_id() << "\n";

        return dbMap.insert({std::this_thread::get_id(), newRocketClient<MockDatabaseAsyncClient>(eb, addr)})
                .first->second->sync_store_message(*client_id, *message);
    }else{
        cout << "Used client for thread ID " << std::this_thread::get_id() << "\n";
        return search->second->sync_store_message(*client_id, *message);
    }
}

mock_message_board::SanitizationHandler::SanitizationHandler() {
    #ifdef LOCALHOST
        addr = folly::SocketAddress("127.0.0.1", 10001, true); // mock database
    #else
        addr = folly::SocketAddress("mock-database", 10001, true); // mock database
    #endif
}

mock_message_board::SanitizationHandler::~SanitizationHandler() {
}
