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

#include "Utils.h"


using apache::thrift::ThriftServer;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using apache::thrift::RequestCallback;
using apache::thrift::ClientReceiveState;
using apache::thrift::RocketClientChannel;
using folly::AsyncSocket;
using folly::ThreadedExecutor;
using namespace std;

void onReply(string message) {
    LOG(INFO) << "client: get response " << message;
}


void onStoreReply(bool resp) {
    LOG(INFO) << "client: get response " << resp;
}

void onError(std::exception const &e) {
    LOG(ERROR) << "client: error: " << e.what();
}


int main(int argc, char *argv[]) {


    google::InitGoogleLogging(argv[0]);
    google::SetCommandLineOption("GLOG_minloglevel", "0"); // INFO
    LOG(INFO) << "Starting Client ...";
    int i = 0;
    int iterations = (argc > ++i) ? stoi(argv[i]) : 5;
    iterations = (iterations > 0) ? iterations : 5;
    string client_id = (argc > ++i) ? argv[i] : "Albert";
    string message = (argc > ++i) ? argv[i] : "TEST MESSAGE";
    // create event runloop, to run on this thread
    folly::EventBase eb;


    #ifdef LOCALHOST
        // folly::SocketAddress addr("127.0.0.1", 10001, true);
        // folly::SocketAddress addr("127.0.0.1", 10011, true); //Middle
        folly::SocketAddress addr("127.0.0.1", 10002, true);
    #else
        // folly::SocketAddress addr("mock-database", 10001, true);
        // folly::SocketAddress addr("mock-database-middle", 10011, true);
        folly::SocketAddress addr("message-service", 10002, true);
    #endif

    // creating client
    auto client = newMessageServiceRocketClient(&eb, addr);
    auto start = std::chrono::system_clock::now();
    std::vector<folly::Future<folly::Unit>> futs;
    for (int32_t i = 0; i < iterations; i++) {
        
        // LOG(INFO) << "client: sending call " << i;
        cout << "client: sending call " << i << endl;

        #ifdef SYNC
            string result; 
            client->sync_find_last_message(result, client_id);
        #else
        // if (message.length() > 0){
            auto f = client->future_find_last_message(client_id);
            futs.push_back(std::move(f).thenValue(onReply).thenError<std::exception>(onError));
        // } else {
            // auto f = client->future_store_message(client_id, message);
            // futs.push_back(std::move(f).thenValue(onStoreReply).thenError<std::exception>(onStoreError));
        // }
        #endif


        LOG(INFO) << "client: sent call " << i;
        // Some computation here
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        cout << "client: sent call " << elapsed_seconds.count() << "s" << endl;
        LOG(INFO) << "client: sent call " << elapsed_seconds.count() << "s" << endl;
    }
    folly::ThreadedExecutor executor;
    std::move(collectAll(futs.begin(), futs.end())).via(&executor).thenValue([&eb](std::vector<folly::Try<folly::Unit>> &&v) {
        LOG(ERROR) << "client: received all responses";
        eb.terminateLoopSoon();
    });
    

    // libevent/epoll loop which keeps main thread from existing.
    eb.loopForever();

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    cout << "client: Finished " << elapsed_seconds.count() << "s" << endl;

    return 0;
}
