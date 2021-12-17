#include "MockDatabase.h"

using namespace std;
using namespace mock_message_board;


void MockDatabaseHandler::find_last_message(string& result, unique_ptr<string> client_id, int64_t query_uid){
    manager.add_entry(query_uid, get_epoch_time_us());
    M_DEBUG_OUT(MOCK_DATABASE_PREFIX << "find_last_message: from " << *client_id << " query: " << (uint64_t)query_uid);
    //=========================================================================

    this_thread::sleep_for(waiting_time);

    if (last_messages.find(*client_id) == last_messages.end()){
        result = "WARNING: This user " + *client_id + " didn't post any message or doesn't exist...";
    } else {
        result = last_messages[*client_id];
    }

    //=========================================================================
    M_DEBUG_OUT(MOCK_DATABASE_PREFIX << "Message found " << result << " from " << *client_id << "s." << " query: " << (uint64_t)query_uid);
    manager.add_entry(query_uid, get_epoch_time_us());
}

bool MockDatabaseHandler::store_message(unique_ptr<string> client_id, unique_ptr<string> message, int64_t query_uid){
    manager.add_entry(query_uid, get_epoch_time_us());
    M_DEBUG_OUT(MOCK_DATABASE_PREFIX << "store_message: " << *message  << " from " << *client_id << " query: " << (uint64_t)query_uid);
    //=========================================================================

    this_thread::sleep_for(waiting_time);

    last_messages[*client_id] = *message;
    
    //=========================================================================
    M_DEBUG_OUT(MOCK_DATABASE_PREFIX << "Message stored: " << *message << " from " << *client_id << " query: " << (uint64_t)query_uid );
    manager.add_entry(query_uid, get_epoch_time_us());
    return true;
}