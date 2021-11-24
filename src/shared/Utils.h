//
// Created by adrien on 11.11.21.
//
#pragma once

#include <folly/io/async/AsyncSocket.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
#include <dep/if/gen-cpp2/MockDatabase.h>
#include <dep/if/gen-cpp2/SanitizationService.h>
#include <dep/if/gen-cpp2/MessageService.h>
#include <iostream>

using folly::AsyncSocket;
using apache::thrift::ThriftServer;
using apache::thrift::RocketClientChannel;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using mock_message_board::MockDatabaseAsyncClient;
using mock_message_board::SanitizationServiceAsyncClient;
using mock_message_board::MessageServiceAsyncClient;

using namespace std;

folly::AsyncTransport::UniquePtr getSocket(folly::EventBase *evb, folly::SocketAddress const &addr);

template <class T>
std::unique_ptr<T> newRocketClient(folly::EventBase *evb, folly::SocketAddress const &addr) {
    auto channel = RocketClientChannel::newChannel(folly::AsyncSocket::newSocket(evb, addr));
    channel->setProtocolId(apache::thrift::protocol::T_COMPACT_PROTOCOL);
    return std::make_unique<T>(std::move(channel));
}

template <typename T>
std::unique_ptr<ThriftServer> newServer(folly::SocketAddress const &addr, shared_ptr<T> handler) {
    auto proc_factory = std::make_shared<ThriftServerAsyncProcessorFactory<T>>(handler);
    auto server = std::make_unique<ThriftServer>();
    server->setAddress(addr);
    server->setProcessorFactory(proc_factory);
    return server;
}
