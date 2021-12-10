//
// Created by adrien on 11.11.21.
//

#include <Utils.h>
#include <dep/if/gen-cpp2/MockDatabase.h>
#include <dep/if/gen-cpp2/SanitizationService.h>
#include <fb303/ServiceData.h>
#include <iostream>
#include <chrono>
#include <thrift/lib/cpp2/GeneratedCodeHelper.h>
#include "MessageService.h"

namespace fb303 = facebook::fb303;

using mock_message_board::MockDatabaseAsyncClient;
using mock_message_board::SanitizationServiceAsyncClient;

/*void mock_message_board::MessageServiceHandler::find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id) {
    // fb303 counter
    const auto p1 = std::chrono::system_clock::now();

    fb303::fbData->setCounter(
        "find_last_message.date",
        (int64_t) std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count()
    );

    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "find_last_message: client_id=" << *client_id);

    dbMapMutex_.lock_shared();
    auto search = dbMap.find(std::this_thread::get_id());
    if(search == dbMap.end()){
        dbMapMutex_.unlock_shared();dbMapMutex_.lock();

        auto *eb = new folly::EventBase();
        M_DEBUG_OUT("\tNew client for thread ID " << std::this_thread::get_id() << ": sending...");

        dbMap.insert({std::this_thread::get_id(), newRocketClient<MockDatabaseAsyncClient>(eb, addr1, MSGSERV_MOCK_TIMEOUT)})
                .first->second->sync_find_last_message(result, *client_id);
        dbMapMutex_.unlock();
    }else{
        M_DEBUG_OUT("\tThread ID " << std::this_thread::get_id() << ": sending...");
        search->second->sync_find_last_message(result, *client_id);
        dbMapMutex_.unlock_shared();
    }

    M_DEBUG_OUT("\tThread ID " << std::this_thread::get_id() << ": received!");
}

bool mock_message_board::MessageServiceHandler::send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) {
    M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "send_message: client_id=" << *client_id << " | message=" << *message);

    sanMapMutex_.lock_shared();
    auto search = sanMap.find(std::this_thread::get_id());
    bool res;
    if(search == sanMap.end()){
        sanMapMutex_.unlock_shared();sanMapMutex_.lock();
        auto *eb = new folly::EventBase();
        M_DEBUG_OUT("\tNew client for thread ID " << std::this_thread::get_id() << ": sending...");

        res = sanMap.insert({std::this_thread::get_id(), newRocketClient<MockDatabaseAsyncClient>(eb, addr1, MSGSERV_SANIT_TIMEOUT)})
        .first->second->sync_store_message(*client_id, *message);
        sanMapMutex_.unlock();
    }else{
        M_DEBUG_OUT("\tThread ID " << std::this_thread::get_id() << ": sending...");
        res = search->second->sync_store_message(*client_id, *message);
        sanMapMutex_.unlock_shared();
    }
    M_DEBUG_OUT("\tThread ID " << std::this_thread::get_id() << ": received!");
    return res;
}*/

folly::Future<std::unique_ptr<::std::string>>
mock_message_board::MessageServiceHandler::future_find_last_message(std::unique_ptr<::std::string> p_client_id) {

    //M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "semifuture_find_last_message: client_id=" << *p_client_id);
    return folly::makeFuture<std::unique_ptr<::std::string>>(make_unique<std::string>("wesdfsd")).thenValue(
            [&](auto result) {
                std::string result2;
                getEventBase()->runInEventBaseThread([&]() {
                    __fbthrift_invocation_send_message.store(apache::thrift::detail::si::InvocationType::SemiFuture,
                                                             std::memory_order_relaxed);
                    gimmeDbConnection()->future_find_last_message(*p_client_id).thenValue([&](const std::string &result) {
                        M_DEBUG_OUT(
                                "\tThread ID " << std::this_thread::get_id() << ": received! client_id: hgjhgjhgjhghj");
                        result2 = result;
                    });
                });
                return make_unique<std::string>(result2);
            });
}

folly::Future<bool>
mock_message_board::MessageServiceHandler::future_send_message(std::unique_ptr<::std::string> p_client_id,
                                                               std::unique_ptr<::std::string> p_message) {

    //M_DEBUG_OUT(MESSAGE_SERVICE_PREFIX << "semifuture_send_message: client_id=" << *p_client_id);

    return folly::makeFuture<bool>(false).thenValue([&](bool result) {
        bool result2;
        getEventBase()->runInEventBaseThread([&]() {
            __fbthrift_invocation_send_message.store(apache::thrift::detail::si::InvocationType::SemiFuture,
                                                     std::memory_order_relaxed);
            gimmeDbConnection()->future_store_message(*p_client_id, *p_message).thenValue([&](bool result) {
                M_DEBUG_OUT("\tThread ID " << std::this_thread::get_id() << ": received! client_id: hgjhgjhgjhghj");
                result2 = result;
            });
        });
        return result2;
    });
}

shared_ptr<MockDatabaseAsyncClient> mock_message_board::MessageServiceHandler::gimmeDbConnection() {

    dbMapMutex_.lock_shared();
    auto search = dbMap->find(std::this_thread::get_id());
    if(search == dbMap->end()){
        dbMapMutex_.unlock_shared();

        auto *eb = new folly::EventBase();
        M_DEBUG_OUT("\tNew client for thread ID " << std::this_thread::get_id());

        dbMapMutex_.lock();
        dbMap->insert({std::this_thread::get_id(), newRocketClient2<MockDatabaseAsyncClient>(eb, addr1, MSGSERV_MOCK_TIMEOUT)});
        dbMapMutex_.unlock();dbMapMutex_.lock_shared();
        search = dbMap->find(std::this_thread::get_id());
    }

    dbMapMutex_.unlock_shared();
    return search->second;
}
