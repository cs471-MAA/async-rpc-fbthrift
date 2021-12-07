//
// Created by adrien on 06.11.21.
//
#include <glog/logging.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <folly/futures/Future.h>
#include <folly/Unit.h>
#include <folly/executors/ThreadedExecutor.h>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <iostream>
#include <fb303/ServiceData.h>

#include <dep/if/gen-cpp2/MessageService.h>
#include "Utils.h"

namespace fb303 = facebook::fb303;

using apache::thrift::ThriftServer;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using apache::thrift::RequestCallback;
using apache::thrift::ClientReceiveState;
using apache::thrift::RocketClientChannel;
using folly::AsyncSocket;
using folly::ThreadedExecutor;
using mock_message_board::MessageServiceAsyncClient;
using namespace std;

int find_success_counter = 0;
int send_success_counter = 0;

const string MOCK_CLIENT_PREFIX = "mock-client     | ";

void onFindReply(string message) {
    M_DEBUG_OUT("\tResponse received: " << message);
    find_success_counter += 1;
}

void onStoreReply(bool resp) {
    M_DEBUG_OUT("\tStore status: " << ((resp) ? "True" : "False"));
    send_success_counter += 1;
}

void onError(std::exception const &e) {
    M_DEBUG_ERR("\tError: " << e.what());
}

void client_summary(std::chrono::duration<double> elapsed_seconds, int iterations);

int main(int argc, char *argv[]) {
    // ======================= INIT ======================= //
    google::InitGoogleLogging(argv[0]);
    google::SetCommandLineOption("GLOG_minloglevel", "0"); // INFO
    M_DEBUG_OUT(MOCK_CLIENT_PREFIX << "Starting Client ...");

    int i = 0;
    int iterations = (argc > ++i) ? stoi(argv[i]) : 5;
    iterations = (iterations > 0) ? iterations : 5;
    string client_id = (argc > ++i) ? argv[i] : "Albert";
    string message = (argc > ++i) ? argv[i] : "TEST MESSAGE";

    // ======================= CLIENT SETUP ======================= //
    folly::SocketAddress addr = M_GET_SOCKET_ADDRESS("mock-database", 10001);
    
    // create event runloop, to run on this thread
    folly::ThreadedExecutor executor;
    folly::EventBase eb;

    // creating client
    auto client = newRocketClient<MockDatabaseAsyncClient>(&eb, addr, 90000);
    auto start = std::chrono::system_clock::now();

    // ======================= SENDING CALLS ======================= //
    std::vector<folly::Future<folly::Unit>> futs;
    for (int32_t i = 0; i < iterations; i++) {
        M_DEBUG_OUT(MOCK_CLIENT_PREFIX << "sending call " << i);

        #ifdef SYNC
            if (i % 2 == 1){
                string result;
                client->sync_find_last_message(result, client_id);
            } else {
                client->sync_store_message(result, client_id);
            }
        #else
            if (i % 2 == 1){
                auto f = client->future_find_last_message(client_id);
                futs.push_back(move(f).thenValue(onFindReply).thenError<std::exception>(onError));
            } else {
                auto f = client->future_store_message(client_id, message);
                futs.push_back(move(f).thenValue(onStoreReply).thenError<std::exception>(onError));
            }
        #endif

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        
        M_DEBUG_OUT(MOCK_CLIENT_PREFIX << "sent call " << i << " at "  << elapsed_seconds.count() << "s");
    }
    
    std::move(collectAll(futs.begin(), futs.end())).via(&executor).thenValue([&eb](std::vector<folly::Try<folly::Unit>> &&v) {
        M_DEBUG_OUT(MOCK_CLIENT_PREFIX << "received all responses");
        eb.terminateLoopSoon();
    });

    // libevent/epoll loop which keeps main thread from existing.
    eb.loopForever();

    // ======================= SUMMARY ======================= //

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    
    client_summary(elapsed_seconds, iterations);

    return 0;
}




void client_summary(std::chrono::duration<double> elapsed_seconds, int iterations){
    
    int find_iterations = iterations/2;
    int send_iterations = iterations - find_iterations;

    M_DEBUG_OUT(MOCK_CLIENT_PREFIX << "Finished after " << elapsed_seconds.count() << "s");
    M_DEBUG_OUT(MOCK_CLIENT_PREFIX << "Received " <<
        send_success_counter << "/" << send_iterations << " successful send message calls.");
    M_DEBUG_OUT(MOCK_CLIENT_PREFIX << "Received " << 
        find_success_counter << "/" << find_iterations  << " successful find message calls.");
    
    fb303::fbData->addStatValue("nb_requests", 1, fb303::SUM);
    auto counters = fb303::fbData->getCounters();
    M_DEBUG_OUT("# of counters=" << static_cast<long unsigned>(counters.size()));
    for (auto item : counters) {
        M_DEBUG_OUT("counter=" << item.first << " | value=" << item.second);
    }
}