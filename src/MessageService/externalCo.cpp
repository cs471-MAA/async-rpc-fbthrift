//
// Created by adrien on 10.12.21.
//

#include "externalCo.h"


externalCo::externalCo() {
}

unique_ptr<MockDatabaseAsyncClient> externalCo::gimmeDbConnection(folly::EventBase *evb) {

    return newRocketClient<MockDatabaseAsyncClient>(evb, M_GET_SOCKET_ADDRESS("mock-database", 10001),
                                                    MSGSERV_MOCK_TIMEOUT);
}

unique_ptr<SanitizationServiceAsyncClient> externalCo::gimmeSanitConnection(folly::EventBase *evb) {
    return newRocketClient<SanitizationServiceAsyncClient>(folly::RequestEventBase::get(), M_GET_SOCKET_ADDRESS("sanitization-service", 10003),
                                                           MSGSERV_SANIT_TIMEOUT);
}

bool externalCo::sync_store_message(const string &p_client_id, const string &p_message, ::std::int64_t p_query_uid) {
    std::string clientIDL = p_client_id;
    std::string messageL = p_message;
    folly::EventBase eb;
    eb.runInEventBaseThread([=]() mutable {
                return gimmeDbConnection()->sync_store_message(clientIDL, messageL, p_query_uid);
            });
}

void externalCo::sync_find_last_message(string &_return, const string &p_client_id, ::std::int64_t p_query_uid) {
    std::string clientIDL = p_client_id;
    std::string returnL;
    folly::RequestEventBase::get()->runInEventBaseThread([=]() mutable {
        gimmeDbConnection()->sync_find_last_message(returnL, clientIDL, p_query_uid);
    });
    _return = returnL;
}

bool externalCo::sync_sanitize_message(const string &p_client_id, const string &p_message, ::std::int64_t p_query_uid) {
    std::string clientIDL = p_client_id;
    std::string messageL = p_message;
    bool res;
    folly::RequestEventBase::get()->runInEventBaseThread([=]() mutable {
        res = gimmeSanitConnection()->sync_sanitize_message(clientIDL, messageL, p_query_uid);
    });
    return res;
}


