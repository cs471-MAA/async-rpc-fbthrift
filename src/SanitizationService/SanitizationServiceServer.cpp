#include <glog/logging.h>
#include <folly/init/Init.h>

#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <folly/executors/ThreadedExecutor.h>
#include <folly/synchronization/Baton.h>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
#include <string>
#include "SanitizationService.h"

using mock_message_board::SanitizationHandler;

int main(int argc, char *argv[]) {
    FLAGS_logtostderr = 1;
    folly::init(&argc, &argv);

    folly::SocketAddress addr("127.0.0.1", 10003, true);
    auto server = newServer(addr, std::make_shared<SanitizationHandler>());
    server->serve();

    return 0;
}
