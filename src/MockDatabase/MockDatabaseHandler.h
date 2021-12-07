#pragma once

#include <glog/logging.h>
#include <unordered_map>
#include <thread>
#include <dep/if/gen-cpp2/MockDatabase.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <fb303/ServiceData.h>
#include "Utils.h"

namespace fb303 = facebook::fb303;

using namespace std;

const string MOCK_DATABASE_PREFIX = "mock-database   | ";

namespace mock_message_board {
    class MockDatabaseHandler : public MockDatabaseSvIf {
    private:
        unordered_map<string, string> last_messages;
        chrono::time_point<chrono::system_clock> start;
        std::chrono::milliseconds waiting_time;
    public:
        
        MockDatabaseHandler(std::chrono::milliseconds waiting_time = 5ms) : start(std::chrono::system_clock::now()), waiting_time(waiting_time) {}

        void find_last_message(string& result, unique_ptr<string> client_id) override{
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            M_DEBUG_OUT(MOCK_DATABASE_PREFIX << "find_last_message: from " << *client_id << " at " << elapsed_seconds.count() << "s.");

            // fb303::fbData->setCounter(
            //     "start.date",
            //     (int64_t) std::chrono::duration_cast<std::chrono::seconds>(end.time_since_epoch()).count()
            // );

            // auto counters = fb303::fbData->getCounters();
            // M_DEBUG_OUT("# of counters=" << static_cast<long unsigned>(counters.size()));
            // for (auto item : counters) {
            //     M_DEBUG_OUT("counter=" << item.first << " | value=" << item.second);
            // }

            this_thread::sleep_for(waiting_time);

            if (last_messages.find(*client_id) == last_messages.end()){
                result = "WARNING: This user " + *client_id + " didn't post any message or doesn't exist...";
            } else {
                result = last_messages[*client_id];
            }

            end = std::chrono::system_clock::now();
            elapsed_seconds = end - start;
            M_DEBUG_OUT("\tMessage found from " << *client_id << " at " << elapsed_seconds.count() << "s.");
        }
        
        bool store_message(unique_ptr<string> client_id, unique_ptr<string> message) override {
            M_DEBUG_OUT(MOCK_DATABASE_PREFIX << "store_message:" << *message << " from " << *client_id);
            this_thread::sleep_for(waiting_time);

            last_messages[*client_id] = *message;
            
            M_DEBUG_OUT("\tMessage stored: " << *message  << " from " << *client_id);
            return true;
        }
        
    };
}
