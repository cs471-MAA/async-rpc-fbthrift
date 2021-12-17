// Created by adrien on 11.11.21.

#include "MessageService.h"

using namespace std;
using namespace mock_message_board;

void MessageServiceHandler::find_last_message(MessageResponse& resp, unique_ptr<string> client_id, int64_t query_uid) {
    manager.add_entry(query_uid, get_epoch_time_us());
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "find_last_message: client_id=" << *client_id);
    //==========================================================================

    string message;
    #ifdef SYNC

        // TASK 1 (Remote)
        thread::id tid = this_thread::get_id();
        auto search = mock_map.find(tid);
        if(search == mock_map.end()){
            auto *eb = new folly::EventBase();
            mock_map.insert({tid, newRocketClient<MockDatabaseAsyncClient>(eb, addr_mock, MSGSERV_MOCK_TIMEOUT)})
                    .first->second->sync_find_last_message(message, *client_id, query_uid);
        }else{
            search->second->sync_find_last_message(message, *client_id, query_uid);
        }

        // TASK 2 (Fake)
        this_thread::sleep_for(waiting_time);
    #else
        // TASK 1 (Remote) Starting 
        auto f = mock->future_find_last_message(*client_id, query_uid);

        // TASK 2 (Fake)
        this_thread::sleep_for(waiting_time);

        // TASK 1 (Remote) Finishing
        f.wait();
        message = f.value();
    #endif

    // response
    resp.query_uid_ref() = query_uid;
    resp.message_ref() = message;

    //==========================================================================
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "Thread ID " << this_thread::get_id() << ": received query: " << (uint64_t)query_uid);
    manager.add_entry(query_uid, get_epoch_time_us());
}

void MessageServiceHandler::send_message(StatusResponse& resp, unique_ptr<string> client_id, unique_ptr<string> message, int64_t query_uid) {
    manager.add_entry(query_uid, get_epoch_time_us());
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "send_message: client_id=" << *client_id << " | message=" << *message);
    //==========================================================================

    bool res = false;
    #ifdef SYNC

        // TASK 1 (Remote)
        thread::id tid = this_thread::get_id();
        auto search = sanit_map.find(tid);
        if(search == sanit_map.end()){
            auto *eb = new folly::EventBase();
            res = sanit_map.insert({tid, newRocketClient<SanitizationServiceAsyncClient>(eb, addr_sanit, MSGSERV_SANIT_TIMEOUT)})
                    .first->second->sync_sanitize_message(*client_id, *message, query_uid);
        }else{
            res = search->second->sync_sanitize_message(*client_id, *message, query_uid);
        }

        // TASK 2 (Fake)
        this_thread::sleep_for(waiting_time);
    #else
        // TASK 1 (Remote) Starting 
        auto f = sanit->future_sanitize_message(*client_id, *message, query_uid);

        // TASK 2 (Fake)
        this_thread::sleep_for(waiting_time);

        // TASK 1 (Remote) Finishing
        f.wait();
        res = f.value();
    #endif

    // response
    resp.ok_ref() = res;
    resp.query_uid_ref() = query_uid;

    //==========================================================================
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "Thread ID " << this_thread::get_id() << ": received query: " << (uint64_t)query_uid);
    manager.add_entry(query_uid, get_epoch_time_us());
}
