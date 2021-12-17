#pragma once

#include <dep/if/gen-cpp2/MessageService.h>
#include "folly/io/async/ScopedEventBaseThread.h"
#include <fb303/ServiceData.h>
#include "common/fb303/cpp/FacebookBase2.h"
#include <Utils.h>
#include "ServerStats.h"

using facebook::fb303::FacebookBase2;

const string MESSAGE_SERVICE_PREFIX = "message-service | ";

namespace mock_message_board {
    class MessageServiceHandler : virtual public MessageServiceSvIf,
                                  public FacebookBase2 {
    public:
        ServerStatsManager manager;
        explicit MessageServiceHandler()
        : FacebookBase2("MessageService"), manager(STATS_FILES_DIR"message_service_stats.csv"){
            addr1 = M_GET_SOCKET_ADDRESS("mock-database", 10001);
            addr2 = M_GET_SOCKET_ADDRESS("sanitization-service", 10003);

            // fb303 counter example
            // const auto p1 = std::chrono::system_clock::now();
            // fb303::fbData->setCounter(
            //     "start.date",
            //     (int64_t) std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count()
            // );
        }
        ~MessageServiceHandler() {
        }

        void find_last_message(MessageResponse& resp, std::unique_ptr<::std::string> client_id, int64_t query_uid) override;
        void send_message(StatusResponse& resp, std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message, int64_t query_uid) override;
    private:
        folly::SocketAddress addr1;
        folly::SocketAddress addr2;
        std::unordered_map<std::thread::id, std::unique_ptr<SanitizationServiceAsyncClient>> sanMap;
        std::unordered_map<std::thread::id, std::unique_ptr<MockDatabaseAsyncClient>> dbMap;
        unordered_map<thread::id, folly::EventBase*> sanEbMap;
        unordered_map<thread::id, folly::EventBase*> dbEbMap;
    };
}
