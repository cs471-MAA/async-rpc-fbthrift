// Created by adrien on 11.11.21.

#include <regex>
#include "SanitizationService.h"

using namespace std;
using namespace mock_message_board;

bool SanitizationHandler::sanitize_message(unique_ptr<string> client_id, unique_ptr<string> message, int64_t query_uid) {
    manager.add_entry(query_uid, get_epoch_time_us());
    M_DEBUG_OUT(SANIT_PREFIX << "sanitize_message: received client_id=" << *client_id << " query: " << (uint64_t)query_uid << " | message=" << *message);
    //==========================================================================

    // TASK 1 (Local)
    regex match_expr("^.*[\\/*$^].*$");
    if (regex_match(*message, match_expr)) {
        return false;
    }

    bool res = false;
    #ifdef SYNC
        // TASK 2 (Remote)
        thread::id tid = this_thread::get_id();
        auto search = mock_map.find(tid);
        if(search == mock_map.end()){
            auto *eb = new folly::EventBase();
            res = mock_map.insert({tid, newRocketClient<MockDatabaseAsyncClient>(eb, addr_mock, SANIT_MOCK_TIMEOUT)})
                        .first->second->sync_store_message(*client_id, *message, query_uid);
        }else{
            res = search->second->sync_store_message(*client_id, *message, query_uid);
        }

        // TASK 3 (Fake)
        this_thread::sleep_for(waiting_time);
    #else
        // TASK 2 (Remote) Starting 
        auto f = mock->future_store_message(*client_id, *message, query_uid);

        // TASK 3 (Fake)
        this_thread::sleep_for(waiting_time);

        // TASK 2 (Remote) Finishing
        f.wait();
        res = f.value();
    #endif

    //==========================================================================
    M_DEBUG_OUT(SANIT_PREFIX << "Thread ID " << this_thread::get_id() << ": received query: " << (uint64_t)query_uid);
    manager.add_entry(query_uid, get_epoch_time_us());
    return res;
}
