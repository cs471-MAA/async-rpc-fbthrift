//
// Created by adrien on 11.11.21.
//

#include <folly/init/Init.h>
#include "MessageService.h"
#include "Utils.h"

using mock_message_board::MessageServiceHandler;

int main(int argc, char** argv) {
    FLAGS_logtostderr = 1;
    folly::init(&argc, &argv);
    #ifdef LOCALHOST
        folly::SocketAddress addr("127.0.0.1", 10002, true);
    #else
        folly::SocketAddress addr("message-service", 10002, true);
    #endif
    auto server = newServer(addr, std::make_shared<MessageServiceHandler>());
    server->serve();

    return 0;
}