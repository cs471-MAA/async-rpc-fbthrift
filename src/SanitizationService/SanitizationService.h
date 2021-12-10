
#include <dep/if/gen-cpp2/SanitizationService.h>
#include "folly/io/async/ScopedEventBaseThread.h"
#include <Utils.h>
#include "ServerStats.h"
const string SANIT_PREFIX = "mock-database   | ";

namespace mock_message_board {

    class SanitizationHandler : public SanitizationServiceSvIf {
        public:
            ServerStatsManager manager;
            SanitizationHandler();
            ~SanitizationHandler() override;

            bool sanitize_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message, int64_t query_uid) override;
        private:
            folly::SocketAddress addr;
            std::unordered_map<std::thread::id, std::unique_ptr<MockDatabaseAsyncClient>> dbMap;
    };

} // namespace mock_message_board