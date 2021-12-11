//
// Created by adrien on 11.11.21.
//
#pragma once

#include <glog/logging.h>
#include <folly/io/async/AsyncSocket.h>
#include <folly/Unit.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/async/HeaderClientChannel.h>
#include <dep/if/gen-cpp2/MockDatabase.h>
#include <dep/if/gen-cpp2/SanitizationService.h>
#include <dep/if/gen-cpp2/MessageService.h>
#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <unistd.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <signal.h>
#include <stdlib.h>

using folly::AsyncSocket;
using apache::thrift::ThriftServer;
using apache::thrift::HeaderClientChannel;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using mock_message_board::MockDatabaseAsyncClient;
using mock_message_board::SanitizationServiceAsyncClient;
using mock_message_board::MessageServiceAsyncClient;

using namespace std;

// #define DEBUG_PRINT # Uncomment to overwrite cmake parameter DEBUG_PRINT
// #define DEBUG_LOG # Uncomment to overwrite cmake parameters DEBUG_LOG
// #define LOCALHOST # Uncomment to overwrite cmake parameters LOCALHOST

#ifdef DEBUG_PRINT
    #define M_COUT(STRING) cout << STRING << endl
    #define M_CERR(STRING) cerr << STRING << endl
#else
    #define M_COUT(STRING) 
    #define M_CERR(STRING)
#endif

#ifdef DEBUG_LOG
    #define M_LOGINFO(STRING) LOG(INFO) << STRING
    #define M_LOGERROR(STRING) LOG(ERROR) << STRING
#else
    #define M_LOGINFO(STRING)
    #define M_LOGERROR(STRING)
#endif

#define M_DEBUG_OUT(STRING) M_COUT(STRING);\
                          M_LOGINFO(STRING);
#define M_DEBUG_ERR(STRING) M_CERR(STRING);\
                          M_LOGERROR(STRING);

#ifdef LOCALHOST
    #define M_GET_SOCKET_ADDRESS(ADDRESS, PORT) folly::SocketAddress("127.0.0.1", PORT, true)
#else
    #define M_GET_SOCKET_ADDRESS(ADDRESS, PORT) folly::SocketAddress(ADDRESS, PORT, true)
#endif

#define STATS_FILES_DIR "/app/src/build/stats_files/"


// https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
string exec(const char* cmd);

uint64_t generate_local_uid();

uint64_t get_query_uid(uint64_t local_uid, uint64_t query_number);

uint64_t get_epoch_time_us();

folly::AsyncTransport::UniquePtr getSocket(folly::EventBase *evb, folly::SocketAddress const &addr);

void sigint_catcher(void (*handler)(int));


template <class T>
std::unique_ptr<T> newRocketClient(folly::EventBase *evb, folly::SocketAddress const &addr, uint32_t timeout = 60000) {
    auto channel = HeaderClientChannel::newChannel(folly::AsyncSocket::newSocket(evb, addr));
    // channel->setTimeout(timeout);
    channel->setTimeout(0);
    return std::make_unique<T>(std::move(channel));
}

template <typename T>
std::unique_ptr<ThriftServer> newServer(folly::SocketAddress const &addr, shared_ptr<T> handler) {
    auto proc_factory = std::make_shared<ThriftServerAsyncProcessorFactory<T>>(handler);
    auto server = std::make_unique<ThriftServer>();
    server->setAddress(addr);
    server->setProcessorFactory(proc_factory);
    // server->setSocketWriteTimeout(8000ms);
    server->setSocketWriteTimeout(0ms);
    
    return server;
}

