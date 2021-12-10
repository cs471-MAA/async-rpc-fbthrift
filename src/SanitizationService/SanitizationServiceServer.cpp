#include <glog/logging.h>
#include <folly/init/Init.h>
#include "Utils.h"
#include "SanitizationService.h"
#include "ServerStats.h"

using mock_message_board::SanitizationHandler;

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

    folly::SocketAddress addr = M_GET_SOCKET_ADDRESS("sanitization-service", 10003);
    auto service_handler = std::make_shared<SanitizationHandler>();
    manager = &(service_handler->manager);
    auto server = newServer(addr, service_handler);
    
    if (iothreads > 0)
        server->setNumIOWorkerThreads(iothreads);
    if (cputhreads > 0)
        server->setNumCPUWorkerThreads(cputhreads);

    // ======================= SERVER STARTS ======================= //

    sigint_catcher(int_handler);

    M_DEBUG_OUT(SANIT_PREFIX << "starts");
    server->serve();

    return 0;
}
