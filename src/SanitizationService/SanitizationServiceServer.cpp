#include <glog/logging.h>
#include <folly/init/Init.h>
#include <fb303/ServiceData.h>

#include "SanitizationService.h"
#include "Utils.h"
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
    chrono::microseconds waiting_time = ((argc > ++i) ? stoi(argv[i]) : 1000) * 1us;
    // ======================= SERVER SETUP ======================= //

    folly::SocketAddress addr = M_SANITIZATION_SERVICE_SOCKET_ADDRESS;
    auto service_handler = std::make_shared<SanitizationHandler>(waiting_time);
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
