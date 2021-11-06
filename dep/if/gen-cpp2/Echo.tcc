/**
 * Autogenerated by Thrift for /home/adrien/Documents/epfl/MA/cs-471/project/dep/if/chatroom.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated @nocommit
 */
#pragma once

#include "if/gen-cpp2/Echo.h"

#include <thrift/lib/cpp2/gen/service_tcc.h>

namespace example { namespace chatroom {
typedef apache::thrift::ThriftPresult<false, apache::thrift::FieldData<1, ::apache::thrift::type_class::string, ::std::string*>> Echo_echo_pargs;
typedef apache::thrift::ThriftPresult<true, apache::thrift::FieldData<0, ::apache::thrift::type_class::string, ::std::string*>> Echo_echo_presult;
template <typename ProtocolIn_, typename ProtocolOut_>
void EchoAsyncProcessor::setUpAndProcess_echo(apache::thrift::ResponseChannelRequest::UniquePtr req, apache::thrift::SerializedCompressedRequest&& serializedRequest, apache::thrift::Cpp2RequestContext* ctx, folly::EventBase* eb, apache::thrift::concurrency::ThreadManager* tm) {
  if (!setUpRequestProcessing(req, ctx, eb, tm, apache::thrift::RpcKind::SINGLE_REQUEST_SINGLE_RESPONSE, iface_)) {
    return;
  }
  auto scope = iface_->getRequestExecutionScope(ctx, apache::thrift::concurrency::NORMAL);
  ctx->setRequestExecutionScope(std::move(scope));
  processInThread(std::move(req), std::move(serializedRequest), ctx, eb, tm, apache::thrift::RpcKind::SINGLE_REQUEST_SINGLE_RESPONSE, &EchoAsyncProcessor::process_echo<ProtocolIn_, ProtocolOut_>, this);
}

template <typename ProtocolIn_, typename ProtocolOut_>
void EchoAsyncProcessor::process_echo(apache::thrift::ResponseChannelRequest::UniquePtr req, apache::thrift::SerializedCompressedRequest&& serializedRequest, apache::thrift::Cpp2RequestContext* ctx, folly::EventBase* eb, apache::thrift::concurrency::ThreadManager* tm) {
  if (!req->getShouldStartProcessing()) {
    apache::thrift::HandlerCallbackBase::releaseRequest(std::move(req), eb);
    return;
  }
  // make sure getRequestContext is null
  // so async calls don't accidentally use it
  iface_->setRequestContext(nullptr);
  Echo_echo_pargs args;
  auto uarg_message = std::make_unique<::std::string>();
  args.get<0>().value = uarg_message.get();
  std::unique_ptr<apache::thrift::ContextStack> ctxStack(this->getContextStack(this->getServiceName(), "Echo.echo", ctx));
  try {
    deserializeRequest<ProtocolIn_>(args, ctx->getMethodName(), std::move(serializedRequest).uncompress(), ctxStack.get());
  }
  catch (const std::exception& ex) {
    folly::exception_wrapper ew(std::current_exception(), ex);
    apache::thrift::detail::ap::process_handle_exn_deserialization<ProtocolOut_>(
        ew, std::move(req), ctx, eb, "echo");
    return;
  }
  auto callback = std::make_unique<apache::thrift::HandlerCallback<std::unique_ptr<::std::string>>>(std::move(req), std::move(ctxStack), return_echo<ProtocolIn_,ProtocolOut_>, throw_wrapped_echo<ProtocolIn_, ProtocolOut_>, ctx->getProtoSeqId(), eb, tm, ctx);
  iface_->async_tm_echo(std::move(callback), std::move(uarg_message));
}

template <class ProtocolIn_, class ProtocolOut_>
apache::thrift::SerializedResponse EchoAsyncProcessor::return_echo(apache::thrift::ContextStack* ctx, ::std::string const& _return) {
  ProtocolOut_ prot;
  Echo_echo_presult result;
  result.get<0>().value = const_cast<::std::string*>(&_return);
  result.setIsSet(0, true);
  return serializeResponse(&prot, ctx, result);
}

template <class ProtocolIn_, class ProtocolOut_>
void EchoAsyncProcessor::throw_wrapped_echo(apache::thrift::ResponseChannelRequest::UniquePtr req,int32_t protoSeqId,apache::thrift::ContextStack* ctx,folly::exception_wrapper ew,apache::thrift::Cpp2RequestContext* reqCtx) {
  if (!ew) {
    return;
  }
  {
    (void)protoSeqId;
    apache::thrift::detail::ap::process_throw_wrapped_handler_error<ProtocolOut_>(
        ew, std::move(req), reqCtx, ctx, "echo");
    return;
  }
}


}} // example::chatroom
