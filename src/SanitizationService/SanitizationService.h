
#include <dep/if/gen-cpp2/SanitizationService.h>
#include "folly/io/async/ScopedEventBaseThread.h"
#include <Utils.h>


namespace mock_message_board {

    class SanitizationHandler : public SanitizationServiceSvIf {
        public:
            SanitizationHandler();
            ~SanitizationHandler() override;

            bool sanitize_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) override;
        private:
            folly::SocketAddress addr;
            folly::ScopedEventBaseThread *clientLoopThread_;
            //std::unique_ptr<MockDatabaseAsyncClient> client;
    };

} // namespace mock_message_board