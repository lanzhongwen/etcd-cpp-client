// Author: Zhongwen(Alan) Lan(runbus@qq.com)
// Created: 2018/07/02
#include "etcd/sclient.h"

#include <chrono>
#include <memory>
#include <thread>
#include <utility>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using etcdserverpb::DeleteRangeRequest;
using etcdserverpb::DeleteRangeResponse;
using etcdserverpb::LeaseGrantRequest;
using etcdserverpb::LeaseGrantResponse;
using etcdserverpb::LeaseKeepAliveRequest;
using etcdserverpb::LeaseKeepAliveResponse;
using etcdserverpb::PutRequest;
using etcdserverpb::PutResponse;
using etcdserverpb::RangeRequest;
using etcdserverpb::RangeResponse;
using etcdserverpb::WatchCreateRequest;
using etcdserverpb::WatchRequest;
using etcdserverpb::WatchResponse;

using mvccpb::KeyValue;

// TODO: configurable
const uint32_t client_conn_timeout = 5;

namespace etcd
{
SClient::SClient(const std::string &etcd_addr)
{
  channel_ = grpc::CreateChannel(etcd_addr, grpc::InsecureChannelCredentials());
  kv_stub_ = KV::NewStub(channel_);
}

SClient::~SClient()
{
  
}

// TODO: timeout and retry
bool SClient::Set(const std::string &key, const std::string &value, int64_t lease_id)
{
  PutRequest req;
  PutResponse resp;
  req.set_key(key);
  req.set_value(value);
  // No need prev_kv in current interface
  // req.set_prev_kv(true);
  req.set_lease(lease_id);

  ClientContext context;
  SetDeadline(context, client_conn_timeout);
  Status status = kv_stub_->Put(&context, req, &resp);

  if (status.ok())
  {
    return true;
  }

  std::cerr << "Set Status: " << status.error_message() << std::endl;

  return false;
}

bool SClient::Delete(const std::string &key)
{
  DeleteRangeRequest req;
  DeleteRangeResponse resp;
  req.set_key(key);
  // No need prev_key in current interface
  // req.set_prev_kv(true);
  ClientContext context;
  SetDeadline(context, client_conn_timeout);
  Status status = kv_stub_->DeleteRange(&context, req, &resp);

  return status.ok();
}

std::string SClient::Get(const std::string &key)
{
  RangeRequest req;
  RangeResponse resp;
  req.set_key(key);
  ClientContext context;
  SetDeadline(context, client_conn_timeout);
  Status status = kv_stub_->Range(&context, req, &resp);
  if (!status.ok())
  {
    return std::string("");
  }
  for (const auto &kv : resp.kvs())
  {
    if (key == kv.key())
    {
      return kv.value();
    }
  }
  return std::string("");
}

int64_t SClient::LeaseGrant(int64_t ttl)
{
  LeaseGrantRequest req;
  LeaseGrantResponse resp;
  req.set_ttl(ttl);
  req.set_id(0);

  ClientContext context;
  SetDeadline(context, client_conn_timeout);
  std::unique_ptr<Lease::Stub> lease_stub = Lease::NewStub(channel_);
  Status status = lease_stub->LeaseGrant(&context, req, &resp);
  if (status.ok())
  {
    return resp.id();
  }
  std::cerr << "LeaseGrant Status: " << status.error_message() << std::endl;
  return 0;
}

Task *SClient::KeepAlive(const std::string &key, int64_t lease_id)
{
  Task *task = new Task();
  task->Start([=]() {
    LeaseKeepAliveRequest req;
    LeaseKeepAliveResponse resp;
    ClientContext context;
    req.set_id(lease_id);
    std::unique_ptr<Lease::Stub> lease_stub = Lease::NewStub(channel_);
    std::unique_ptr<ClientReaderWriter<LeaseKeepAliveRequest, LeaseKeepAliveResponse>> stream = lease_stub->LeaseKeepAlive(&context);
    task->SetKeepStream(stream.get());
    stream->Write(req);
    while (task->IsStop() == false)
    {
      if (stream->Read(&resp))
      {
        int64_t ttl = resp.ttl();
        if (ttl == 0)
        {
          // TODO: Do we want to quit here?
          std::cout << "Warning: KeepAlive TTL: " << ttl << std::endl;
          break;
        }
        // Note: ttl is in seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(ttl * 1000 / 2));
        if (task->IsStop()) {
          break;
        }
        int64_t id = map_.GetLeaseId(key);
        if (id == 0)
        {
          std::cerr << "Failed to get lease id with key: " << key << std::endl;
        }
        req.set_id(id);
        stream->Write(req);
      }
      else
      {
        // TODO: Shall we exit here?
        std::cerr << "KeepAlive: Read failed" << std::endl;
        break;
      }
    }
    task->Stop();
    std::cout << "Exiting KeepAlive...: thread id: " << std::this_thread::get_id() << std::endl;
  });

  return task;
}

Task *SClient::WatchGuard(const std::string &key, const std::string &value, int64_t ttl)
{
  Task *task = new Task();
  task->Start([=]() {
    ClientContext context;
    std::unique_ptr<Watch::Stub> watch_stub = Watch::NewStub(channel_);
    std::unique_ptr<ClientReaderWriter<WatchRequest, WatchResponse>> stream = watch_stub->Watch(&context);
    task->SetWatchStream(stream.get());
    WatchRequest req;
    WatchResponse resp;
    WatchCreateRequest create_req;
    create_req.set_key(key);
    create_req.set_prev_kv(false);
    req.mutable_create_request()->CopyFrom(create_req);
    stream->Write(req);
    while (task->IsStop() == false)
    {
      if (stream->Read(&resp))
      {
        for (const auto &ev : resp.events())
        {
          std::cout << "WatchGuard: Read Event: " << ev.type() << std::endl;
          if (ev.type() == ::mvccpb::Event::DELETE)
          {
            std::cout << "Caught: DELETE by " << std::this_thread::get_id() << std::endl;
            if (task->IsStop())
            {
              std::cout << "Stop set already: Exiting...: thread id: "
                        << std::this_thread::get_id() << std::endl;
              break;
            }
            // When DELETE found, re-register
            int64_t lease_id(0);
            while (((lease_id = LeaseGrant(ttl)) == 0))
            {
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
              std::cerr << "WatchGuard: LeaseGrant retry" << std::endl;
            }

            if (!map_.Set(key, lease_id))
            {
              std::cerr << "Failed to set lease id with key: " << key << std::endl;
            }

            while (!Set(key, value, lease_id))
            {
              std::cerr << "WatchGuard: Set failed: key: " << key
                        << " | value: " << value
                        << " | lease_id: " << lease_id << std::endl;
              std::cerr << "WatchGuard: Set retry..." << std::endl;
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
          }
        }
      }
      else
      {
        std::cerr << "WatchGuard: Read failed" << std::endl;
        break;
      }
    }
    task->Stop();
    std::cout << "Exiting WatchGuard...: thread id: " << std::this_thread::get_id() << std::endl;
  });
  return task;
}

bool SClient::SRegister(const std::string &key, const std::string &value, int64_t ttl)
{
  // TODO: Shall we remove same key directly or update key instead
  map_.Delete(key);
  int64_t lease_id = LeaseGrant(ttl);
  if (lease_id == 0)
  {
    std::cerr << "LeaseGrant Failed" << std::endl;
    return false;
  }
  bool ret = Set(key, value, lease_id);
  if (!ret)
  {
    std::cerr << "Set Failed" << std::endl;
    return false;
  }

  Task *lease_task = KeepAlive(key, lease_id);
  Task *watch_task = WatchGuard(key, value, ttl);

  map_.Insert(key, lease_task, watch_task, lease_id);

  return true;
}

void SClient::SetDeadline(ClientContext &context, uint32_t timeout_seconds)
{
  std::chrono::system_clock::time_point deadline =
      std::chrono::system_clock::now() + std::chrono::seconds(timeout_seconds);
  context.set_deadline(deadline);
}

} // namespace etcd
