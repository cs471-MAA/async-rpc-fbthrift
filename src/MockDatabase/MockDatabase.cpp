#include <glog/logging.h>
#include <folly/init/Init.h>

#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/synchronization/Baton.h>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
#include <fb303/ServiceData.h>
#include "MockDatabaseHandler.h"
#include "Utils.h"
#include "ServerStats.h"

namespace fb303 = facebook::fb303;

using apache::thrift::ThriftServer;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using apache::thrift::RequestCallback;
using apache::thrift::ClientReceiveState;
using apache::thrift::RocketClientChannel;
using folly::AsyncSocket;
using folly::ThreadedExecutor;
using mock_message_board::MockDatabaseHandler;

using namespace std;
ServerStatsManager* manager;
void int_handler(int s){
    delete manager;
    exit(1); 
}

int main(int argc, char *argv[]) {
    // ======================= INIT ======================= //
    // FLAGS_logtostderr = 1;
    folly::init(&argc, &argv);

    int i = 0;
    int iothreads = (argc > ++i) ? stoi(argv[i]) : 0;
    int cputhreads = (argc > ++i) ? stoi(argv[i]) : 0;

    // ======================= SERVER SETUP ======================= //
    
    folly::SocketAddress addr = M_GET_SOCKET_ADDRESS("mock-database", 10001);
    auto service_handler = std::make_shared<MockDatabaseHandler>();
    manager = &(service_handler->manager);
    auto server = newServer(addr, service_handler);
    
    if (iothreads > 0)
        server->setNumIOWorkerThreads(iothreads);
    if (cputhreads > 0)
        server->setNumCPUWorkerThreads(cputhreads);
    
    // ======================= SERVER STARTS ======================= //

    sigint_catcher(int_handler);

    M_DEBUG_OUT(MOCK_DATABASE_PREFIX << " starts");
    server->serve();
}
