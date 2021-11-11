//
// Created by adrien on 06.11.21.
//
#include <glog/logging.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <folly/futures/Future.h>
#include <folly/Unit.h>
#include <folly/executors/ThreadedExecutor.h>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>

#include "MockDatabaseHandler.h"
#include "Utils.h"


using apache::thrift::ThriftServer;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using apache::thrift::RequestCallback;
using apache::thrift::ClientReceiveState;
using apache::thrift::RocketClientChannel;
using folly::AsyncSocket;
using folly::ThreadedExecutor;
using mock_message_board::MockDatabaseHandler;
using mock_message_board::MockDatabaseAsyncClient;
using namespace std;

void onReply(string message) {
    LOG(INFO) << "client: get response " << message;
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
    folly::SocketAddress addr("127.0.0.1", 10002, true);

    // creating client
    auto client = newMockDatabaseRocketClient(&eb, addr);
    std::vector<folly::Future<folly::Unit>> futs;
    for (int32_t i = 0; i < 5; i++) {
        auto start = std::chrono::system_clock::now();
        LOG(INFO) << "client: sending call " << i;
        auto f = client->future_find_last_message("Albert");
        futs.push_back(std::move(f).thenValue(onReply).thenError<std::exception>(onError));
        LOG(INFO) << "client: sent call " << i;
        // Some computation here
        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        LOG(INFO) << "client: sent call " << elapsed_seconds.count() << "s";
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
