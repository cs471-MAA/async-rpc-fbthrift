
#include "if/gen-cpp2/MessageService.h"
#include "folly/io/async/ScopedEventBaseThread.h"
#include <Utils.h>

namespace mock_message_board {
    class MessageServiceHandler : public MessageServiceSvIf {
    public:
        MessageServiceHandler();
        ~MessageServiceHandler() override;

        void find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id) override;

        bool send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) override;
    private:
        folly::SocketAddress addr1;
        folly::SocketAddress addr2;
        folly::ScopedEventBaseThread *clientLoopThread_;
        //std::unique_ptr<MockDatabaseAsyncClient> client;
    };
}