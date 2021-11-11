//
// Created by adrien on 11.11.21.
//

#include <Utils.h>
#include "MessageService.h"

void mock_message_board::MessageServiceHandler::find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id) {

    folly::EventBase *eb = getEventBase();
    folly::SocketAddress addr("mock-database", 10001, true);

    // creating client
    auto client = newMockDatabaseRocketClient(&eb, addr);

    client->find_last_message("Albert");
}

bool mock_message_board::MessageServiceHandler::send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) {
    return false;
}

mock_message_board::MessageServiceHandler::MessageServiceHandler(bool test) : bidule(test) {

}

mock_message_board::MessageServiceHandler::~MessageServiceHandler() {

}
