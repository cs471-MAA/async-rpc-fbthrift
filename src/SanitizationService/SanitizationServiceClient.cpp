//
// Created by adrien on 06.11.21.
//
#include <glog/logging.h>
#include <folly/init/Init.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <folly/futures/Future.h>
#include <folly/Unit.h>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/synchronization/Baton.h>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>

#include <if/gen-cpp2/SanitizationService.h>

using apache::thrift::ThriftServer;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using apache::thrift::RequestCallback;
using apache::thrift::ClientReceiveState;
using apache::thrift::RocketClientChannel;
using folly::AsyncSocket;
using folly::ThreadedExecutor;
using mock_message_board::SanitizationServiceAsyncClient;
using namespace std;

folly::AsyncTransport::UniquePtr getSocket(folly::EventBase *evb, folly::SocketAddress const &addr) {
    folly::AsyncTransport::UniquePtr sock(new AsyncSocket(evb, addr, 120, true));
    return sock;
}

static std::unique_ptr<SanitizationServiceAsyncClient> newRocketClient(folly::EventBase *evb, folly::SocketAddress const &addr) {

    auto channel = RocketClientChannel::newChannel(getSocket(evb, addr));
    channel->setProtocolId(apache::thrift::protocol::T_COMPACT_PROTOCOL);
    return std::make_unique<SanitizationServiceAsyncClient>(std::move(channel));
}

void onReply(bool response) {
    if (response)
        LOG(ERROR) << "client: get response: true";
    else
        LOG(ERROR) << "client: get response: false";
}

void onError(std::exception const &e) {
    LOG(ERROR) << "client: error: " << e.what();
}

int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    google::SetCommandLineOption("GLOG_minloglevel", "0"); // INFO
    LOG(INFO) << "Starting Client ...";

    // create event runloop, to run on this thread
    folly::EventBase eb;
    folly::SocketAddress addr("sanitization-service", 10002, true);

    // test messages
    const string messages[5] = {
        "Test message", "Test messa$ge", "Test* message", "Test mess\\age", "Hello! :')"
    };

    // creating client
    auto client = newRocketClient(&eb, addr);
    std::vector<folly::Future<folly::Unit>> futs;
    for (int32_t i = 0; i < 5; i++) {
        auto start = std::chrono::system_clock::now();

        LOG(INFO) << "client: sending call " << i;
        auto f = client->future_sanitize_message("client-1", messages[i]);
        futs.push_back(std::move(f).thenValue(onReply).thenError<std::exception>(onError));
        LOG(INFO) << "client: sent call " << i;
        
        // Some computation here
        
        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        LOG(INFO) << "client: sent call in " << elapsed_seconds.count() << "s";
    }
    folly::ThreadedExecutor executor;
    std::move(collectAll(futs.begin(), futs.end())).via(&executor).thenValue([&eb](std::vector<folly::Try<folly::Unit>> &&v) {
        LOG(INFO) << "client: received all responses";
        eb.terminateLoopSoon();
    });

    // libevent/epoll loop which keeps main thread from existing.
    eb.loopForever();

    return 0;
}
