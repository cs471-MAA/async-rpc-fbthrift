#pragma once

#include <glog/logging.h>
#include <regex>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
#include <folly/init/Init.h>
#include <folly/futures/Future.h>
#include <folly/Unit.h>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/synchronization/Baton.h>
#include <if/gen-cpp2/SanitizationService.h>
#include <if/gen-cpp2/MockDatabaseAsyncClient.h>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>


using apache::thrift::ThriftServer;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using apache::thrift::RequestCallback;
using apache::thrift::ClientReceiveState;
using apache::thrift::RocketClientChannel;
using folly::AsyncSocket;
using mock_message_board::MockDatabaseAsyncClient;

folly::AsyncTransport::UniquePtr getSocket(folly::EventBase *evb, folly::SocketAddress const &addr) {
    folly::AsyncTransport::UniquePtr sock(new AsyncSocket(evb, addr, 120, true));
    return sock;
}

static std::unique_ptr<MockDatabaseAsyncClient> newRocketClient(folly::EventBase *evb, folly::SocketAddress const &addr) {
    auto channel = RocketClientChannel::newChannel(getSocket(evb, addr));
    channel->setProtocolId(apache::thrift::protocol::T_COMPACT_PROTOCOL);
    return std::make_unique<MockDatabaseAsyncClient>(std::move(channel));
}

void onReply(bool response) {
    LOG(INFO) << "server: mock database responded: " << response;
}

namespace mock_message_board {

    class SanitizationHandler : virtual public SanitizationServiceSvIf {
        private:
            std::unique_ptr<MockDatabaseAsyncClient> client;

        public:
            SanitizationHandler() {
                // create event runloop, to run on this thread
                folly::EventBase eb;
                #ifdef LOCALHOST
                    folly::SocketAddress addr("127.0.0.1", 10001, true);
                #else
                    folly::SocketAddress addr("mock-database", 10001, true);
                #endif
                // create client
                this->client = newRocketClient(&eb, addr);
            }

            bool sanitize_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) override {
                std::regex match_expr("^.*[\\/*$^].*$");
                if (std::regex_match(*message, match_expr)) {
                    LOG(INFO) << "server: message is invalid";
                    return false;
                }

                // TODO: bool store_message(1: string client_id, 2: string message)
                auto fut = this->client->future_store_message(*client_id, *message);
                std::move(fut).thenValue(onReply);

                return true;
            }
    };

} // namespace mock_message_board