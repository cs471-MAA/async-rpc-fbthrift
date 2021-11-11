#include <glog/logging.h>
#include <folly/init/Init.h>

#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/synchronization/Baton.h>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
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
// using mock_message_board::MockDatabaseAsyncClient;
using namespace std;

int main(int argc, char *argv[]) {
    // FLAGS_logtostderr = 1;
    folly::init(&argc, &argv);


    int i = 0;
    int iothreads = (argc > ++i) ? stoi(argv[i]) : 0;
    int cputhreads = (argc > ++i) ? stoi(argv[i]) : 0;

    // folly::SocketAddress addr("mock-database", 10001, true);
    folly::SocketAddress addr("127.0.0.1", 10001, true);
    
    auto server = newServer(addr, std::make_shared<MockDatabaseHandler>());
    if (iothreads > 0)
        server->setNumIOWorkerThreads(iothreads);
    if (cputhreads > 0)
        server->setNumCPUWorkerThreads(cputhreads);
    
    LOG(INFO) << "server: starts";
    server->serve();
}
