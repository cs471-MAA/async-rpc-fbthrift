#pragma once


#include <unordered_map>
#include <dep/if/gen-cpp2/MockDatabase.h>
#include <chrono>
#include "Utils.h"
#include "ServerStats.h"

namespace fb303 = facebook::fb303;

using namespace std;

const string MOCK_DATABASE_PREFIX = "mock-database   | ";

namespace mock_message_board {
    class MockDatabaseHandler : public MockDatabaseSvIf {
    private:
        unordered_map<string, string> last_messages;
        chrono::time_point<chrono::system_clock> start;
        chrono::microseconds waiting_time;
    public:
        ServerStatsManager manager;
        
        MockDatabaseHandler(chrono::microseconds waiting_time = 5000us) : 
            start(chrono::system_clock::now()), 
            waiting_time(waiting_time), 
            manager(STATS_FILES_DIR"mock_database_stats.csv") {}

        void find_last_message(string& result, unique_ptr<string> client_id, int64_t query_uid) override;
        bool store_message(unique_ptr<string> client_id, unique_ptr<string> message, int64_t query_uid) override;
    };
}
