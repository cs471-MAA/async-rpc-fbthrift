#pragma once

#include <dep/if/gen-cpp2/MessageService.h>
#include "folly/io/async/ScopedEventBaseThread.h"
#include <Utils.h>
#include "common/fb303/cpp/FacebookBase2.h"

using facebook::fb303::FacebookBase2;


namespace mock_message_board {
    class MessageServiceHandler : virtual public MessageServiceSvIf,
                                  public FacebookBase2 {
    public:
        explicit MessageServiceHandler()
        : FacebookBase2("MessageService"), clientLoopThread_(new folly::ScopedEventBaseThread())  {
            #ifdef LOCALHOST
                addr1 = folly::SocketAddress("127.0.0.1", 10001, true); // mock database
                addr2 = folly::SocketAddress("127.0.0.1", 10003, true); // sanitization
            #else 
                addr1 = folly::SocketAddress("mock-database", 10001, true); // mock database
                addr2 = folly::SocketAddress("sanitization-service", 10003, true); // sanitization
            #endif
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