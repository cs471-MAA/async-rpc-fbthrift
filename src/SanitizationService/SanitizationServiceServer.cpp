#include <glog/logging.h>
#include <folly/init/Init.h>
#include "Utils.h"
#include "SanitizationService.h"

using mock_message_board::SanitizationHandler;

int main(int argc, char *argv[]) {
    // ======================= INIT ======================= //
    // FLAGS_logtostderr = 1;
    folly::init(&argc, &argv);

    int i = 0;
    int iothreads = (argc > ++i) ? stoi(argv[i]) : 0;
    int cputhreads = (argc > ++i) ? stoi(argv[i]) : 0;

    // ======================= SERVER SETUP ======================= //

    folly::SocketAddress addr = M_GET_SOCKET_ADDRESS("sanitization-service", 10003);

    auto server = newServer(addr, std::make_shared<SanitizationHandler>());
    
    if (iothreads > 0)
        server->setNumIOWorkerThreads(iothreads);
    if (cputhreads > 0)
        server->setNumCPUWorkerThreads(cputhreads);

    // ======================= SERVER STARTS ======================= //
    M_DEBUG_OUT(SANIT_PREFIX << "starts");
    server->serve();

    return 0;
}
