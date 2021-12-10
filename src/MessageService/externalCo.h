//
// Created by adrien on 10.12.21.
//

#ifndef THRIFTTRANSPORT_CPP2_EXAMPLE_EXTERNALCO_H
#define THRIFTTRANSPORT_CPP2_EXAMPLE_EXTERNALCO_H

//https://github.com/RobotGuy2/Jarvis/blob/0bad428bb763fe404d01db5d9e08ee33a8f3776c/lucida/imagematching/opencv_imm/server/IMMHandler.cpp
#include <folly/io/async/EventBase.h>
#include <Utils.h>

const uint32_t MSGSERV_MOCK_TIMEOUT = 20000;
const uint32_t MSGSERV_SANIT_TIMEOUT = 30000;

class externalCo {
public:
    explicit externalCo();

    bool sync_store_message(const ::std::string& p_client_id, const ::std::string& p_message, ::std::int64_t p_query_uid);
    void sync_find_last_message(::std::string& _return, const ::std::string& p_client_id, ::std::int64_t p_query_uid);

    bool sync_sanitize_message(const ::std::string& p_client_id, const ::std::string& p_message, ::std::int64_t p_query_uid);

private:
    unique_ptr<MockDatabaseAsyncClient> gimmeDbConnection(folly::EventBase *evb);
    unique_ptr<SanitizationServiceAsyncClient> gimmeSanitConnection(folly::EventBase *evb);
    folly::EventBase* eb_;
};


#endif //THRIFTTRANSPORT_CPP2_EXAMPLE_EXTERNALCO_H
