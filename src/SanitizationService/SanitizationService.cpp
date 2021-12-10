//
// Created by adrien on 11.11.21.
//

#include <Utils.h>
#include <regex>
#include <dep/if/gen-cpp2/MessageService.h>
#include "SanitizationService.h"

uint32_t SANIT_MOCK_TIMEOUT = 20000;

using mock_message_board::MessageServiceAsyncClient;

bool mock_message_board::SanitizationHandler::sanitize_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message, int64_t query_uid) {
    manager.add_entry(query_uid, get_epoch_time_us());
    M_DEBUG_OUT(SANIT_PREFIX << "sanitize_message: client_id=" << *client_id << " query_uid=" << query_uid << " | message=" << *message);

    std::regex match_expr("^.*[\\/*$^].*$");
    if (std::regex_match(*message, match_expr)) {
        return false;
    }

    bool res;
    eb.runInEventBaseThread([&]() {
        res = dbClient->sync_store_message(*client_id, *message, query_uid);
        M_DEBUG_OUT(SANIT_PREFIX << "sync_store_message " << query_uid << ": received!");
    });
    M_DEBUG_OUT("\tThread ID " << std::this_thread::get_id() << ": received!");
    
    manager.add_entry(query_uid, get_epoch_time_us());
    return res;
}

mock_message_board::SanitizationHandler::SanitizationHandler() :
manager(STATS_FILES_DIR"sanitization_service_stats.csv"){
    addr = M_GET_SOCKET_ADDRESS("mock-database", 10001);
    dbClient = newRocketClient<MockDatabaseAsyncClient>(&eb, addr, SANIT_MOCK_TIMEOUT);
    }

mock_message_board::SanitizationHandler::~SanitizationHandler() {
    eb.terminateLoopSoon();
}
