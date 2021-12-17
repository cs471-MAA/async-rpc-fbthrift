
#include <thread>
#include <folly/io/async/ScopedEventBaseThread.h>
#include "Utils.h"
#include "dep/if/gen-cpp2/SanitizationService.h"
#include "ServerStats.h"

const string SANIT_PREFIX = "mock-database   | ";

#define SANIT_MOCK_TIMEOUT 20000

namespace mock_message_board {

    class SanitizationHandler : public SanitizationServiceSvIf {
        public:
            ServerStatsManager manager;
            chrono::microseconds waiting_time;
            SanitizationHandler(chrono::microseconds waiting_time= 1000us):
                manager(STATS_FILES_DIR"sanitization_service_stats.csv"),
                waiting_time(waiting_time)
            {
                addr_mock = M_MOCK_DATABASE_SOCKET_ADDRESS;
            
                #ifndef SYNC
                    mock_eb = make_shared<folly::EventBase>();
                    mock = newRocketClient<MockDatabaseAsyncClient>(mock_eb.get(), addr_mock, SANIT_MOCK_TIMEOUT);
                    thread mock_eb_loop_thread([](shared_ptr<folly::EventBase> eb) {
                        eb.get()->loopForever();
                    }, mock_eb);
        
                    mock_eb_loop_thread.detach();
                #endif
            }
            ~SanitizationHandler() {}

            bool sanitize_message(unique_ptr<string> client_id, unique_ptr<string> message, int64_t query_uid) override;
        private:            
            folly::SocketAddress addr_mock;
            #ifdef SYNC
                unordered_map<thread::id, unique_ptr<MockDatabaseAsyncClient>> mock_map;
            #else
                shared_ptr<MockDatabaseAsyncClient> mock;
                shared_ptr<folly::EventBase> mock_eb;
            #endif
    };


} // namespace mock_message_board