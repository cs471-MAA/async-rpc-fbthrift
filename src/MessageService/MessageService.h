#pragma once

#include <dep/if/gen-cpp2/MessageService.h>
#include "folly/io/async/ScopedEventBaseThread.h"
#include <fb303/ServiceData.h>
#include <Utils.h>
#include "common/fb303/cpp/FacebookBase2.h"
// #include <fb303/BaseService.h>

namespace fb303 = facebook::fb303;

using facebook::fb303::FacebookBase2;

const string MESSAGE_SERVICE_PREFIX = "message-service | ";
const uint32_t MSGSERV_MOCK_TIMEOUT = 20000;
const uint32_t MSGSERV_SANIT_TIMEOUT = 30000;


namespace mock_message_board {
    class MessageServiceHandler : virtual public MessageServiceSvIf,
                                //   public facebook::fb303::BaseService {
                                  public FacebookBase2 {
    public:
        explicit MessageServiceHandler()
        : FacebookBase2("MessageService") {

            // fb303 counter example
            const auto p1 = std::chrono::system_clock::now();
            fb303::fbData->setCounter(
                "start.date",
                (int64_t) std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count()
            );
        }
        ~MessageServiceHandler() {}

        // RPCs
        //void find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id) override;
        folly::Future<std::unique_ptr<::std::string>> future_find_last_message(std::unique_ptr<::std::string> p_client_id) override;
        //bool send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) override;
        folly::Future<bool> future_send_message(std::unique_ptr<::std::string> p_client_id, std::unique_ptr<::std::string> p_message) override;

        private:
        std::shared_ptr<std::unordered_map<std::thread::id, std::shared_ptr<MockDatabaseAsyncClient>>> dbMap =
                std::make_shared<std::unordered_map<std::thread::id, std::shared_ptr<MockDatabaseAsyncClient>>>();
        mutable std::shared_mutex dbMapMutex_;
        shared_ptr<MockDatabaseAsyncClient>  gimmeDbConnection();
        const folly::SocketAddress addr1 = M_GET_SOCKET_ADDRESS("mock-database", 10001);
        const folly::SocketAddress addr2 =  M_GET_SOCKET_ADDRESS("sanitization-service", 10003);
    };
}
