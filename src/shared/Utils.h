//
// Created by adrien on 11.11.21.
//
#include <folly/io/async/AsyncSocket.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
#include "if/gen-cpp2/MockDatabase.h"

using folly::AsyncSocket;
using apache::thrift::ThriftServer;
using apache::thrift::ThriftServerAsyncProcessorFactory;
using apache::thrift::RocketClientChannel;
using mock_message_board::MockDatabaseAsyncClient;

using namespace std;

folly::AsyncTransport::UniquePtr getSocket(folly::EventBase *evb, folly::SocketAddress const &addr) {
    folly::AsyncTransport::UniquePtr sock(new AsyncSocket(evb, addr, 120, true));
    return sock;
}

static std::unique_ptr<MockDatabaseAsyncClient> newMockDatabaseRocketClient(folly::EventBase *evb, folly::SocketAddress const &addr) {

    auto channel = RocketClientChannel::newChannel(getSocket(evb, addr));
    channel->setProtocolId(apache::thrift::protocol::T_COMPACT_PROTOCOL);
    return std::make_unique<MockDatabaseAsyncClient>(std::move(channel));
}

template <typename T>
std::unique_ptr<ThriftServer> newServer(folly::SocketAddress const &addr, shared_ptr<T> handler) {
    auto proc_factory = std::make_shared<ThriftServerAsyncProcessorFactory<T>>(handler);
    auto server = std::make_unique<ThriftServer>();
    server->setAddress(addr);
    server->setProcessorFactory(proc_factory);
    return server;
}
