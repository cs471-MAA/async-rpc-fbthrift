#include <glog/logging.h>
#include <folly/init/Init.h>
#include "Utils.h"
#include "SanitizationService.h"

using mock_message_board::SanitizationHandler;

int main(int argc, char *argv[]) {
    // FLAGS_logtostderr = 1;
    folly::init(&argc, &argv);

    int i = 0;
    int iothreads = (argc > ++i) ? stoi(argv[i]) : 0;
    int cputhreads = (argc > ++i) ? stoi(argv[i]) : 0;

    folly::SocketAddress addr("127.0.0.1", 10003, true);

    auto server = newServer(addr, std::make_shared<SanitizationHandler>());
    
    if (iothreads > 0)
        server->setNumIOWorkerThreads(iothreads);
    if (cputhreads > 0)
        server->setNumCPUWorkerThreads(cputhreads);

    LOG(INFO) << "server: starts";
    server->serve();

    return 0;
}
