//
// Created by adrien on 11.11.21.
//

#include <glog/logging.h>
#include <folly/init/Init.h>
#include <fb303/ServiceData.h>

#include "MessageService.h"
#include "Utils.h"
#include "ServerStats.h"

namespace fb303 = facebook::fb303;

ServerStatsManager* manager;
void int_handler(int s){
    delete manager;
    exit(1); 
}

using mock_message_board::MessageServiceHandler;

int main(int argc, char** argv) {
    // ======================= INIT ======================= //

    // FLAGS_logtostderr = 1;
    folly::init(&argc, &argv);
    
    int i = 0;
    int iothreads = (argc > ++i) ? stoi(argv[i]) : 0;
    int cputhreads = (argc > ++i) ? stoi(argv[i]) : 0;
    chrono::microseconds waiting_time = ((argc > ++i) ? stoi(argv[i]) : 1000) * 1us;
    // ======================= SERVER SETUP ======================= //

    folly::SocketAddress addr = M_MESSAGE_SERVICE_SOCKET_ADDRESS;
    auto service_handler = std::make_shared<MessageServiceHandler>(waiting_time);
    manager = &(service_handler->manager);
    auto server = newServer(addr, service_handler);

    if (iothreads > 0)
        server->setNumIOWorkerThreads(iothreads);
    if (cputhreads > 0)
        server->setNumCPUWorkerThreads(cputhreads);


    // fb303 stuff
    auto counters = fb303::fbData->getCounters();
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "# of counters=" << static_cast<long unsigned>(counters.size()));
    for (auto item : counters) {
        M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "counter=" << item.first << " | value=" << item.second);
    }

    // ======================= SERVER STARTS ======================= //
    sigint_catcher(int_handler);

    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "start");
    // run server
    server->serve();
    
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "end");

    return 0;
}