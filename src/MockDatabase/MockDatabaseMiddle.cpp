#include <glog/logging.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <folly/futures/Future.h>
#include <folly/Unit.h>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/init/Init.h>
#include <folly/synchronization/Baton.h>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>

#include "MockDatabaseMiddleHandler.h"


using apache::thrift::ThriftServer;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using apache::thrift::RequestCallback;
using apache::thrift::ClientReceiveState;
using apache::thrift::RocketClientChannel;
using folly::AsyncSocket;
using folly::ThreadedExecutor;
using mock_message_board::MockDatabaseMiddleHandler;
using namespace std;

void onReply(string message) {
    LOG(INFO) << "client: get response " << message;
}

void onError(std::exception const &e) {
    LOG(ERROR) << "client: error: " << e.what();
}

int main(int argc, char *argv[]) {
    LOG(INFO) << "Starting Middle ...";
    // FLAGS_logtostderr = 1;
    folly::init(&argc, &argv);

    folly::SocketAddress addr("mock-database-middle", 10011, true);
    // folly::SocketAddress addr("127.0.0.1", 10011, true);
    auto middle_server = newServer(addr, std::make_shared<MockDatabaseMiddleHandler>());
    LOG(INFO) << "server: Middle starts";
    
    middle_server->serve();

    return 0;
}
