
#include <dep/if/gen-cpp2/SanitizationService.h>
#include "folly/io/async/ScopedEventBaseThread.h"
#include <Utils.h>
#include "ServerStats.h"
const string SANIT_PREFIX = "sanit-service   | ";

namespace mock_message_board {

    class SanitizationHandler : public SanitizationServiceSvIf {
        public:
            ServerStatsManager manager;
            SanitizationHandler();
            ~SanitizationHandler() override;

            bool sanitize_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message, int64_t query_uid) override;
        private:
            folly::SocketAddress addr;
            folly::EventBase eb;
            std::unique_ptr<MockDatabaseAsyncClient> dbClient;
    };

} // namespace mock_message_board