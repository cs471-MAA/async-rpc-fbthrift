//
// Created by adrien on 11.11.21.
//

#include <folly/init/Init.h>
#include "MessageService.h"
#include "Utils.h"

int main(int argc, char** argv) {
    FLAGS_logtostderr = 1;
    folly::init(&argc, &argv);

    auto chatroom_server = newServer<MessageServiceHandler>(666);
    std::thread t([&] {
        LOG(INFO) << "ChatRoom Server running on port: " << FLAGS_chatroom_port;
        chatroom_server->serve();
    });

    auto echo_server = newServer<EchoHandler>(FLAGS_echo_port);
    LOG(INFO) << "Echo Server running on port: " << FLAGS_echo_port;
    echo_server->serve();

    return 0;
}