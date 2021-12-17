#pragma once

#include <dep/if/gen-cpp2/MessageService.h>
#include "folly/io/async/ScopedEventBaseThread.h"
#include <fb303/ServiceData.h>
#include "common/fb303/cpp/FacebookBase2.h"
#include <Utils.h>
#include "ServerStats.h"

using facebook::fb303::FacebookBase2;

#define MSGSERV_MOCK_TIMEOUT 20000
#define MSGSERV_SANIT_TIMEOUT 30000

const string MESSAGE_SERVICE_PREFIX = "message-service | ";

namespace mock_message_board {
    class MessageServiceHandler : virtual public MessageServiceSvIf,
                                  public FacebookBase2 {
    public:
        ServerStatsManager manager;
        explicit MessageServiceHandler()
        : FacebookBase2("MessageService"), manager(STATS_FILES_DIR"message_service_stats.csv"){
            addr_mock = M_GET_SOCKET_ADDRESS("mock-database", 10001);
            addr_sanit = M_GET_SOCKET_ADDRESS("sanitization-service", 10003);
            sanit_eb = make_shared<folly::EventBase>();
            mock_eb = make_shared<folly::EventBase>();
            
            mock = newRocketClient<MockDatabaseAsyncClient>(mock_eb.get(), addr_mock, MSGSERV_MOCK_TIMEOUT);
            sanit = newRocketClient<SanitizationServiceAsyncClient>(sanit_eb.get(), addr_sanit, MSGSERV_SANIT_TIMEOUT);
            
            thread mock_eb_loop_thread([](shared_ptr<folly::EventBase> eb) {
                eb.get()->loopForever();
            }, mock_eb);
            thread sanit_eb_loop_thread([](shared_ptr<folly::EventBase> eb) {
                eb.get()->loopForever();
            }, sanit_eb); 
            mock_eb_loop_thread.detach();
            sanit_eb_loop_thread.detach();
        }
        ~MessageServiceHandler() {
        }

        void find_last_message(MessageResponse& resp, std::unique_ptr<::std::string> client_id, int64_t query_uid) override;
        void send_message(StatusResponse& resp, std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message, int64_t query_uid) override;
    private:
        folly::SocketAddress addr_mock;
        folly::SocketAddress addr_sanit;
        shared_ptr<SanitizationServiceAsyncClient> sanit;
        shared_ptr<MockDatabaseAsyncClient> mock;
        shared_ptr<folly::EventBase> sanit_eb;
        shared_ptr<folly::EventBase> mock_eb;

    };
}
