//
// Created by adrien on 11.11.21.
//

#include <glog/logging.h>
#include <folly/init/Init.h>
#include <fb303/ServiceData.h>

#include "MessageService.h"
#include "Utils.h"

namespace fb303 = facebook::fb303;

using mock_message_board::MessageServiceHandler;

int main(int argc, char** argv) {
    // FLAGS_logtostderr = 1;
    folly::init(&argc, &argv);

    int i = 0;
    int iothreads = (argc > ++i) ? stoi(argv[i]) : 0;
    int cputhreads = (argc > ++i) ? stoi(argv[i]) : 0;
    
    #ifdef LOCALHOST
        folly::SocketAddress addr("127.0.0.1", 10002, true);
    #else
        folly::SocketAddress addr("message-service", 10002, true);
    #endif

    auto server = newServer(addr, std::make_shared<MessageServiceHandler>());

    if (iothreads > 0)
        server->setNumIOWorkerThreads(iothreads);
    if (cputhreads > 0)
        server->setNumCPUWorkerThreads(cputhreads);

    LOG(INFO) << "server: start";

    // fb303 stuff
    auto counters = fb303::fbData->getCounters();
    std::cout << "server: # of counters=" << static_cast<long unsigned>(counters.size()) << std::endl;
    for (auto item : counters) {
        std::cout << "server: counter=" << item.first << " | value=" << item.second << std::endl;
    }

    auto dyn_counters = fb303::fbData->getDynamicCounters();
    std::cout << "server: # of dynamic counters=" << static_cast<long unsigned>(counters.size()) << std::endl;
    for (auto item : counters) {
        std::cout << "server: dynamic counter=" << item.first << " | value=" << item.second << std::endl;
    }

    // run server
    server->serve();
    
    LOG(INFO) << "server: end";

    return 0;
}