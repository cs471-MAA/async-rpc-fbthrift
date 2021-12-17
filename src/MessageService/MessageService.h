#pragma once

#include <folly/io/async/ScopedEventBaseThread.h>
#include <dep/if/gen-cpp2/MessageService.h>
#include <dep/if/gen-cpp2/MockDatabase.h>
#include <dep/if/gen-cpp2/SanitizationService.h>
#include <fb303/ServiceData.h>
#include "common/fb303/cpp/FacebookBase2.h"
#include "Utils.h"
#include "ServerStats.h"
#ifdef SYNC
    #include <unordered_map>
#endif
using facebook::fb303::FacebookBase2;

#define MSGSERV_MOCK_TIMEOUT 20000
#define MSGSERV_SANIT_TIMEOUT 30000

const string MESSAGE_SERVICE_PREFIX = "message-service | ";

namespace mock_message_board {
    class MessageServiceHandler : virtual public MessageServiceSvIf,
                                  public FacebookBase2 {
    public:
        ServerStatsManager manager;
        chrono::microseconds waiting_time;
        explicit MessageServiceHandler(chrono::microseconds waiting_time= 1000us) : 
            FacebookBase2("MessageService"), 
            manager(STATS_FILES_DIR"message_service_stats.csv"),
            waiting_time(waiting_time) 
        {
            addr_mock = M_MOCK_DATABASE_SOCKET_ADDRESS;
            addr_sanit = M_SANITIZATION_SERVICE_SOCKET_ADDRESS;

            #ifndef SYNC
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
            #endif
        }
        ~MessageServiceHandler() {
        }

        void find_last_message(MessageResponse& resp, unique_ptr<string> client_id, int64_t query_uid) override;
        void send_message(StatusResponse& resp, unique_ptr<string> client_id, unique_ptr<string> message, int64_t query_uid) override;
    private:
        folly::SocketAddress addr_mock;
        folly::SocketAddress addr_sanit;

        #ifdef SYNC
            unordered_map<thread::id, unique_ptr<SanitizationServiceAsyncClient>> sanit_map;
            unordered_map<thread::id, unique_ptr<MockDatabaseAsyncClient>> mock_map;
        #else
            shared_ptr<SanitizationServiceAsyncClient> sanit;
            shared_ptr<MockDatabaseAsyncClient> mock;
            shared_ptr<folly::EventBase> sanit_eb;
            shared_ptr<folly::EventBase> mock_eb;
        #endif

    };
}
