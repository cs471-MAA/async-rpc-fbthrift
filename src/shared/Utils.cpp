#include "Utils.h"
#include <thrift/lib/cpp2/async/RocketClientChannel.h>
#include <iostream>

using apache::thrift::RocketClientChannel;

folly::AsyncTransport::UniquePtr getSocket(folly::EventBase *evb, folly::SocketAddress const &addr) {
    folly::AsyncTransport::UniquePtr sock(new AsyncSocket(evb, addr, 120, true));
    return sock;
}
