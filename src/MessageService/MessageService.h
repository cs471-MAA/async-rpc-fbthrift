#pragma once

#include <dep/if/gen-cpp2/MessageService.h>
#include "folly/io/async/ScopedEventBaseThread.h"
#include <fb303/ServiceData.h>
#include "common/fb303/cpp/FacebookBase2.h"
#include <Utils.h>
#include "ServerStats.h"
// #include <fb303/BaseService.h>

namespace fb303 = facebook::fb303;

using facebook::fb303::FacebookBase2;

const string MESSAGE_SERVICE_PREFIX = "message-service | ";

namespace mock_message_board {
    class MessageServiceHandler : virtual public MessageServiceSvIf,
                                //   public facebook::fb303::BaseService {
                                  public FacebookBase2 {
    public:
        ServerStatsManager manager;
        explicit MessageServiceHandler()
        : FacebookBase2("MessageService"), manager("message_service_stats.csv"){
        // : BaseService("MessageService") {
            addr1 = M_GET_SOCKET_ADDRESS("mock-database", 10001);
            addr2 = M_GET_SOCKET_ADDRESS("sanitization-service", 10003);

            // fb303 counter example
            const auto p1 = std::chrono::system_clock::now();
            fb303::fbData->setCounter(
                "start.date",
                (int64_t) std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count()
            );
            // fb303::fbData->setCounter(
            //     "start.date",
            //     (int64_t) 2
            // );
            // fb303::fbData->addStatValue("nb_requests", 1, fb303::SUM);
            // fb303::fbData->addStatValue("nb_requests", 1, fb303::SUM);
            // fb303::fbData->addStatValue("nb_requests", 1, fb303::SUM);
        }
        ~MessageServiceHandler() {
        }

        // Fix some inheritance issues
        // std::unique_ptr<apache::thrift::AsyncProcessor> getProcessor() {
        //     return this->MessageServiceSvIf::getProcessor();
        // }
        // std::vector<apache::thrift::ServiceHandler*> getServiceHandlers() {
        //     return this->MessageServiceSvIf::getServiceHandlers();
        // }
        // apache::thrift::AsyncProcessorFactory::CreateMethodMetadataResult createMethodMetadata() {
        //     return this->MessageServiceSvIf::createMethodMetadata();
        // }
        // typedef facebook::fb303::cpp2::BaseServiceAsyncProcessor ProcessorType;

        // RPCs
        void find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id, int64_t query_uid) override;
        bool send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message, int64_t query_uid) override;
    private:
        folly::SocketAddress addr1;
        folly::SocketAddress addr2;
        std::unordered_map<std::thread::id, std::unique_ptr<SanitizationServiceAsyncClient>> sanMap;
        std::unordered_map<std::thread::id, std::unique_ptr<MockDatabaseAsyncClient>> dbMap;
    };
}
