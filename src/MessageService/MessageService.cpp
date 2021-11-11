//
// Created by adrien on 11.11.21.
//

#include "MessageService.h"

void mock_message_board::MessageServiceHandler::find_last_message(::std::string &, std::unique_ptr<::std::string>) {

}

bool mock_message_board::MessageServiceHandler::send_message(std::unique_ptr<::std::string>, std::unique_ptr<::std::string>) {
    return false;
}

mock_message_board::MessageServiceHandler::MessageServiceHandler() : bidule(true) {

}

mock_message_board::MessageServiceHandler::~MessageServiceHandler() {

}
