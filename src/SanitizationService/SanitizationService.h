
#include <dep/if/gen-cpp2/SanitizationService.h>
#include "folly/io/async/ScopedEventBaseThread.h"
#include <Utils.h>
#include "ServerStats.h"
const string SANIT_PREFIX = "mock-database   | ";

#define SANIT_MOCK_TIMEOUT 20000

namespace mock_message_board {

    class SanitizationHandler : public SanitizationServiceSvIf {
        public:
            ServerStatsManager manager;
            
            SanitizationHandler():
                manager(STATS_FILES_DIR"sanitization_service_stats.csv")
            {
                addr_mock = M_GET_SOCKET_ADDRESS("mock-database", 10001);
                mock_eb = make_shared<folly::EventBase>();
            
                mock = newRocketClient<MockDatabaseAsyncClient>(mock_eb.get(), addr_mock, SANIT_MOCK_TIMEOUT);
                
                thread mock_eb_loop_thread([](shared_ptr<folly::EventBase> eb) {
                    eb.get()->loopForever();
                }, mock_eb);
    
                mock_eb_loop_thread.detach();
            }
            ~SanitizationHandler() {}

            bool sanitize_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message, int64_t query_uid) override;
        private:            
            folly::SocketAddress addr_mock;
            shared_ptr<MockDatabaseAsyncClient> mock;
            shared_ptr<folly::EventBase> mock_eb;
    };


} // namespace mock_message_board