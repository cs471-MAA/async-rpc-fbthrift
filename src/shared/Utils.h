//
// Created by adrien on 11.11.21.
//
#pragma once

#include <folly/io/async/AsyncSocket.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include "if/gen-cpp2/MockDatabase.h"
#include "if/gen-cpp2/SanitizationService.h"

using folly::AsyncSocket;
using apache::thrift::ThriftServer;
using mock_message_board::MockDatabaseAsyncClient;
using mock_message_board::SanitizationServiceAsyncClient;
using apache::thrift::ThriftServerAsyncProcessorFactory;

using namespace std;

folly::AsyncTransport::UniquePtr getSocket(folly::EventBase *evb, folly::SocketAddress const &addr);

std::unique_ptr<MockDatabaseAsyncClient> newMockDatabaseRocketClient(folly::EventBase *evb, folly::SocketAddress const &addr);
std::unique_ptr<SanitizationServiceAsyncClient> newSanitizationServiceRocketClient(folly::EventBase *evb, folly::SocketAddress const &addr);

template <typename T>
std::unique_ptr<ThriftServer> newServer(folly::SocketAddress const &addr, shared_ptr<T> handler) {
    auto proc_factory = std::make_shared<ThriftServerAsyncProcessorFactory<T>>(handler);
    auto server = std::make_unique<ThriftServer>();
    server->setAddress(addr);
    server->setProcessorFactory(proc_factory);
    return server;
}
