//
// Created by adrien on 06.11.21.
//
#include <glog/logging.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <folly/futures/Future.h>
#include <folly/Unit.h>
#include <folly/executors/ThreadedExecutor.h>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>
#include <fb303/ServiceData.h>

#include <dep/if/gen-cpp2/MessageService.h>
#include "Utils.h"
#include "ServerStats.h"

using folly::ThreadedExecutor;
using mock_message_board::MessageServiceAsyncClient;
using mock_message_board::MessageResponse;
using mock_message_board::StatusResponse;
using namespace std;

const string CLIENT_PREFIX = "client          | ";

int find_success_counter = 0;
int send_success_counter = 0;
ServerStatsManager client_stat_manager(STATS_FILES_DIR"client_stats.csv");


void int_handler(int s){
    delete &client_stat_manager;
    exit(1); 
}

void onFindReply(MessageResponse resp) {
    M_DEBUG_OUT(CLIENT_PREFIX << "query: " << (uint64_t)*resp.query_uid_ref() << "response received: " << *resp.message_ref());
    find_success_counter += 1;
    uint64_t epoch_time_ms = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
    client_stat_manager.add_entry((uint64_t)*resp.query_uid_ref(), epoch_time_ms);
}

void onSendReply(StatusResponse resp) {
    M_DEBUG_OUT(CLIENT_PREFIX << "Send message status: " << ((*resp.ok_ref()) ? "True" : "False"));
    send_success_counter += 1;
    uint64_t epoch_time_ms = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
    client_stat_manager.add_entry((uint64_t)*resp.query_uid_ref(), epoch_time_ms);
}

void onError(std::exception const &e) {
    M_DEBUG_ERR(CLIENT_PREFIX << "Error: " << e.what());
}

void client_summary(std::chrono::duration<double> elapsed_seconds, int iterations);

int main(int argc, char *argv[]) {
    // ======================= INIT ======================= //
    google::InitGoogleLogging(argv[0]);
    google::SetCommandLineOption("GLOG_minloglevel", "0"); // INFO
    M_DEBUG_OUT(CLIENT_PREFIX << "Starting Client ...");
    
    int i = 0;
    int iterations = (argc > ++i) ? stoi(argv[i]) : 10;
    iterations = (iterations > 0) ? iterations : 10;
    string client_id = (argc > ++i) ? argv[i] : "Albert";
    string message = (argc > ++i) ? argv[i] : "TEST MESSAGE";
    float waiting_time = ((argc > ++i) ? stof(argv[i]) : 10000.f);
    float decreasing_factor = ((argc > ++i) ? stof(argv[i]) : 1.f);
    uint32_t timeout = (argc > ++i) ? stoi(argv[i]) : 90000; //ms

    // ======================= CLIENT SETUP ======================= //

    folly::SocketAddress addr = M_GET_SOCKET_ADDRESS("message-service", 10002);

    // create event runloop, to run on this thread
    folly::ThreadedExecutor executor;

    shared_ptr<folly::EventBase> eb = make_shared<folly::EventBase>();

    // creating client
    auto client = newRocketClient<MessageServiceAsyncClient>(eb.get(), addr, timeout);
    uint64_t client_uid = generate_local_uid();

    // ======================= SENDING CALLS ======================= //
    
    std::thread ebloop_thread([](shared_ptr<folly::EventBase> eb) {
            eb.get()->loopForever();
        }, eb);
    ebloop_thread.detach();

    sigint_catcher(int_handler);
    std::vector<folly::Future<folly::Unit>> futs;
    auto start = std::chrono::system_clock::now();
    for (int32_t i = 0; i < iterations; i++) {
        uint64_t query_uid = get_query_uid(client_uid, i);
        M_DEBUG_OUT(CLIENT_PREFIX << "sending call " << i);
        client_stat_manager.add_entry(query_uid, get_epoch_time_us());

        #ifdef SYNC
            if (i % 2 == 1){
                string result;
                client->sync_find_last_message(result, client_id, query_uid);
            } else {
                client->sync_send_message(client_id, message, query_uid);
            }
        #else
            if (i % 2 == 1){
                auto f = client->future_find_last_message(client_id, query_uid);
                futs.push_back(std::move(f).thenValue(onFindReply).thenError<std::exception>(onError));
            } else {
                auto f = client->future_send_message(client_id, message, query_uid);
                futs.push_back(std::move(f).thenValue(onSendReply).thenError<std::exception>(onError));
            }
            
        #endif

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        M_DEBUG_OUT(CLIENT_PREFIX << "sent call " << i << " at " << elapsed_seconds.count() << "s");
        this_thread::sleep_for(waiting_time*1us);
        waiting_time *= decreasing_factor;
        M_DEBUG_OUT(waiting_time)
    }
        
    auto f = std::move(collectAll(futs.begin(), futs.end())).via(&executor).thenValue([&eb](std::vector<folly::Try<folly::Unit>> &&v) {
        M_DEBUG_OUT(CLIENT_PREFIX << "received all responses");
        eb.get()->terminateLoopSoon();
    });

    f.wait();
    


    // ======================= SUMMARY ======================= //

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    client_summary(elapsed_seconds, iterations);
    delete &client_stat_manager;
    return 0;
}

void client_summary(std::chrono::duration<double> elapsed_seconds, int iterations){
    int find_iterations = iterations/2;
    int send_iterations = iterations - find_iterations;

    M_DEBUG_OUT(CLIENT_PREFIX << "Finished after " << elapsed_seconds.count() << "s");
    M_DEBUG_OUT(CLIENT_PREFIX << "Received " <<
        send_success_counter << "/" << send_iterations << " successful send message calls.");
    M_DEBUG_OUT(CLIENT_PREFIX << "Received " << 
        find_success_counter << "/" << find_iterations  << " successful find message calls.");
}
