#pragma once

#include <dep/if/gen-cpp2/MessageService.h>
#include "folly/io/async/ScopedEventBaseThread.h"
#include <fb303/ServiceData.h>
#include <Utils.h>
#include "common/fb303/cpp/FacebookBase2.h"
// #include <fb303/BaseService.h>

namespace fb303 = facebook::fb303;

using facebook::fb303::FacebookBase2;


namespace mock_message_board {
    class MessageServiceHandler : virtual public MessageServiceSvIf,
                                //   public facebook::fb303::BaseService {
                                  public FacebookBase2 {
    public:
        explicit MessageServiceHandler()
        : FacebookBase2("MessageService"), clientLoopThread_(new folly::ScopedEventBaseThread())  {
        // : BaseService("MessageService"), clientLoopThread_(new folly::ScopedEventBaseThread())  {
            #ifdef LOCALHOST
                addr1 = folly::SocketAddress("127.0.0.1", 10001, true); // mock database
                addr2 = folly::SocketAddress("127.0.0.1", 10003, true); // sanitization
            #else 
                addr1 = folly::SocketAddress("mock-database", 10001, true); // mock database
                addr2 = folly::SocketAddress("sanitization-service", 10003, true); // sanitization
            #endif

            // fb303 counter
            const auto p1 = std::chrono::system_clock::now();
            fb303::fbData->setCounter(
                "start.date",
                (int64_t) std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count()
            );
            fb303::fbData->setCounter(
                "start.date",
                (int64_t) 2
            );
            fb303::fbData->addStatValue("nb_requests", 1, fb303::SUM);
            fb303::fbData->addStatValue("nb_requests", 1, fb303::SUM);
            fb303::fbData->addStatValue("nb_requests", 1, fb303::SUM);
        }
        ~MessageServiceHandler() {
            delete clientLoopThread_;
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
        void find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id) override;
        bool send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) override;
    private:
        folly::SocketAddress addr1;
        folly::SocketAddress addr2;
        folly::ScopedEventBaseThread *clientLoopThread_;
    };
} // namespace mock_message_board