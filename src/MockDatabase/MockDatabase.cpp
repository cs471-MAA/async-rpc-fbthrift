#include <glog/logging.h>
#include <folly/init/Init.h>

#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/synchronization/Baton.h>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
#include <string>
#include "MockDatabaseHandler.h"

using apache::thrift::ThriftServer;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using apache::thrift::RequestCallback;
using apache::thrift::ClientReceiveState;
using apache::thrift::RocketClientChannel;
using folly::AsyncSocket;
using folly::ThreadedExecutor;
using mock_message_board::MockDatabaseHandler;
// using mock_message_board::MockDatabaseAsyncClient;
using namespace std;

std::unique_ptr<ThriftServer> newServer(folly::SocketAddress const &addr) {
    auto handler = std::make_shared<MockDatabaseHandler>();
    auto proc_factory = std::make_shared<ThriftServerAsyncProcessorFactory<MockDatabaseHandler>>(handler);
    auto server = std::make_unique<ThriftServer>();
    server->setAddress(addr);
    server->setProcessorFactory(proc_factory);
    return server;
}

int main(int argc, char *argv[]) {
    LOG(INFO) << "Starting test ...";
    FLAGS_logtostderr = 1;
    folly::init(&argc, &argv);

    folly::SocketAddress addr("mock-database", 10001, true);
    auto server = newServer(addr);
    LOG(INFO) << "server: starts";
    server->serve();
}
