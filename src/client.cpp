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
#include "./ExampleHandler.h"
#include "shared.h"


using apache::thrift::ThriftServer;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using apache::thrift::RequestCallback;
using apache::thrift::ClientReceiveState;
using apache::thrift::RocketClientChannel;
using folly::AsyncSocket;
using folly::ThreadedExecutor;
using example::ExampleHandler;
using example::ExampleServiceAsyncClient;


folly::AsyncTransport::UniquePtr getSocket(folly::EventBase *evb, folly::SocketAddress const &addr) {
    folly::AsyncTransport::UniquePtr sock(new AsyncSocket(evb, addr, 120, true));
    return sock;
}

static std::unique_ptr<ExampleServiceAsyncClient> newRocketClient(folly::EventBase *evb, folly::SocketAddress const &addr) {

    auto channel = RocketClientChannel::newChannel(getSocket(evb, addr));
    channel->setProtocolId(apache::thrift::protocol::T_COMPACT_PROTOCOL);
    return std::make_unique<ExampleServiceAsyncClient>(std::move(channel));
}

void onReply(int32_t number) {
    LOG(INFO) << "client: get response " << number;
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
    folly::SocketAddress addr("server", thrift_port, true);

    // creating client
    auto client = newRocketClient(&eb, addr);
    std::vector<folly::Future<folly::Unit>> futs;
    for (int32_t i = 10; i < 100000; i++) {
        //LOG(INFO) << "client: send number " << i;

        auto f = client->future_get_number(i);
        futs.push_back(std::move(f).thenValue(onReply).thenError<std::exception>(onError));
    }
    folly::ThreadedExecutor executor;
    std::move(collectAll(futs.begin(), futs.end())).via(&executor).thenValue([&eb](std::vector<folly::Try<folly::Unit>> &&v) {
        LOG(ERROR) << "client: received all responses";
        eb.terminateLoopSoon();
    });

    // libevent/epoll loop which keeps main thread from existing.
    eb.loopForever();

    return 0;
}
