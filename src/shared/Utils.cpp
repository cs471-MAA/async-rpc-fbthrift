#include "Utils.h"
#include <thrift/lib/cpp2/async/RocketClientChannel.h>

using apache::thrift::RocketClientChannel;

folly::AsyncTransport::UniquePtr getSocket(folly::EventBase *evb, folly::SocketAddress const &addr) {
    folly::AsyncTransport::UniquePtr sock(new AsyncSocket(evb, addr, 120, true));
    return sock;
}

std::unique_ptr<MockDatabaseAsyncClient> newMockDatabaseRocketClient(folly::EventBase *evb, folly::SocketAddress const &addr) {

    auto channel = RocketClientChannel::newChannel(folly::AsyncSocket::newSocket(evb, addr));
    channel->setProtocolId(apache::thrift::protocol::T_COMPACT_PROTOCOL);
    return std::make_unique<MockDatabaseAsyncClient>(std::move(channel));
}

std::unique_ptr<SanitizationServiceAsyncClient> newSanitizationServiceRocketClient(folly::EventBase *evb, folly::SocketAddress const &addr) {

    auto channel = RocketClientChannel::newChannel(folly::AsyncSocket::newSocket(evb, addr));
    channel->setProtocolId(apache::thrift::protocol::T_COMPACT_PROTOCOL);
    return std::make_unique<SanitizationServiceAsyncClient>(std::move(channel));
}
