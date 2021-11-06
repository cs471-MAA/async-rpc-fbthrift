/**
 * Autogenerated by Thrift for /home/adrien/Documents/epfl/MA/cs-471/project/dep/if/chatroom.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated @nocommit
 */

#include "if/gen-cpp2/Echo.h"
#include "if/gen-cpp2/Echo.tcc"
#include "if/gen-cpp2/chatroom_metadata.h"
#include <thrift/lib/cpp2/gen/service_cpp.h>

namespace example { namespace chatroom {
std::unique_ptr<apache::thrift::AsyncProcessor> EchoSvIf::getProcessor() {
  return std::make_unique<EchoAsyncProcessor>(this);
}

EchoSvIf::CreateMethodMetadataResult EchoSvIf::createMethodMetadata() {
  return ::apache::thrift::detail::ap::createMethodMetadataMap<EchoAsyncProcessor>();
}


void EchoSvIf::echo(::std::string& /*_return*/, std::unique_ptr<::std::string> /*message*/) {
  apache::thrift::detail::si::throw_app_exn_unimplemented("echo");
}

folly::SemiFuture<std::unique_ptr<::std::string>> EchoSvIf::semifuture_echo(std::unique_ptr<::std::string> p_message) {
  auto expected{apache::thrift::detail::si::InvocationType::SemiFuture};
  __fbthrift_invocation_echo.compare_exchange_strong(expected, apache::thrift::detail::si::InvocationType::Sync, std::memory_order_relaxed);
  auto ret = std::make_unique<::std::string>();
  echo(*ret, std::move(p_message));
  return folly::makeSemiFuture(std::move(ret));
}

folly::Future<std::unique_ptr<::std::string>> EchoSvIf::future_echo(std::unique_ptr<::std::string> p_message) {
  auto expected{apache::thrift::detail::si::InvocationType::Future};
  __fbthrift_invocation_echo.compare_exchange_strong(expected, apache::thrift::detail::si::InvocationType::SemiFuture, std::memory_order_relaxed);
  return apache::thrift::detail::si::future(semifuture_echo(std::move(p_message)), getInternalKeepAlive());
}

void EchoSvIf::async_tm_echo(std::unique_ptr<apache::thrift::HandlerCallback<std::unique_ptr<::std::string>>> callback, std::unique_ptr<::std::string> p_message) {
  // It's possible the coroutine versions will delegate to a future-based
  // version. If that happens, we need the RequestParams arguments to be
  // available to the future through the thread-local backchannel, so we create
  // a RAII object that sets up RequestParams and clears them on destruction.
  apache::thrift::detail::si::AsyncTmPrep asyncTmPrep(this, callback.get());
  auto invocationType = __fbthrift_invocation_echo.load(std::memory_order_relaxed);
  try {
    switch (invocationType) {
      case apache::thrift::detail::si::InvocationType::AsyncTm:
      {
        __fbthrift_invocation_echo.compare_exchange_strong(invocationType, apache::thrift::detail::si::InvocationType::Future, std::memory_order_relaxed);
        FOLLY_FALLTHROUGH;
      }
      case apache::thrift::detail::si::InvocationType::Future:
      {
        auto fut = future_echo(std::move(p_message));
        apache::thrift::detail::si::async_tm_future(std::move(callback), std::move(fut));
        return;
      }
      case apache::thrift::detail::si::InvocationType::SemiFuture:
      {
        auto fut = semifuture_echo(std::move(p_message));
        apache::thrift::detail::si::async_tm_semifuture(std::move(callback), std::move(fut));
        return;
      }
      case apache::thrift::detail::si::InvocationType::Sync:
      {
        ::std::string _return;
        echo(_return, std::move(p_message));
        callback->result(_return);
        return;
      }
      default:
      {
        folly::assume_unreachable();
      }
    }
  } catch (...) {
    callback->exception(std::current_exception());
  }
}

void EchoSvNull::echo(::std::string& /*_return*/, std::unique_ptr<::std::string> /*message*/) {}



const char* EchoAsyncProcessor::getServiceName() {
  return "Echo";
}

void EchoAsyncProcessor::getServiceMetadata(apache::thrift::metadata::ThriftServiceMetadataResponse& response) {
  ::apache::thrift::detail::md::ServiceMetadata<EchoSvIf>::gen(response);
}

void EchoAsyncProcessor::processSerializedCompressedRequest(apache::thrift::ResponseChannelRequest::UniquePtr req, apache::thrift::SerializedCompressedRequest&& serializedRequest, apache::thrift::protocol::PROTOCOL_TYPES protType, apache::thrift::Cpp2RequestContext* context, folly::EventBase* eb, apache::thrift::concurrency::ThreadManager* tm) {
  apache::thrift::detail::ap::process(this, std::move(req), std::move(serializedRequest), protType, context, eb, tm);
}

void EchoAsyncProcessor::processSerializedCompressedRequestWithMetadata(apache::thrift::ResponseChannelRequest::UniquePtr req, apache::thrift::SerializedCompressedRequest&& serializedRequest, const apache::thrift::AsyncProcessorFactory::MethodMetadata& methodMetadata, apache::thrift::protocol::PROTOCOL_TYPES protType, apache::thrift::Cpp2RequestContext* context, folly::EventBase* eb, apache::thrift::concurrency::ThreadManager* tm) {
  apache::thrift::detail::ap::process(this, std::move(req), std::move(serializedRequest), methodMetadata, protType, context, eb, tm);
}

const EchoAsyncProcessor::ProcessMap& EchoAsyncProcessor::getOwnProcessMap() {
  return kOwnProcessMap_;
}

const EchoAsyncProcessor::ProcessMap EchoAsyncProcessor::kOwnProcessMap_ {
  {"echo", {&EchoAsyncProcessor::setUpAndProcess_echo<apache::thrift::CompactProtocolReader, apache::thrift::CompactProtocolWriter>, &EchoAsyncProcessor::setUpAndProcess_echo<apache::thrift::BinaryProtocolReader, apache::thrift::BinaryProtocolWriter>}},
};

}} // example::chatroom
