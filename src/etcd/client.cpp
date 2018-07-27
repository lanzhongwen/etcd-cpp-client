// Author: Zhongwen(Alan) Lan(runbus@qq.com)
// Created: 2018/07/02
#include "etcd/client.h"

#include <chrono>
#include <memory>
#include <utility>


using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;

using etcdserverpb::PutRequest;
using etcdserverpb::PutResponse;
using etcdserverpb::DeleteRangeRequest;
using etcdserverpb::DeleteRangeResponse;
using etcdserverpb::LeaseGrantRequest;
using etcdserverpb::LeaseGrantResponse;
using etcdserverpb::LeaseKeepAliveRequest;
using etcdserverpb::LeaseKeepAliveResponse;
using etcdserverpb::RangeRequest;
using etcdserverpb::RangeResponse;
using etcdserverpb::WatchRequest;
using etcdserverpb::WatchResponse;
using etcdserverpb::WatchCreateRequest;

using mvccpb::KeyValue;

namespace etcd {
Client::Client(const std::string & etcd_addr) {
  std::shared_ptr<Channel> channel = grpc::CreateChannel(etcd_addr, grpc::InsecureChannelCredentials()); 
  kv_stub_ = KV::NewStub(channel);
  watch_stub_ = Watch::NewStub(channel);
  lease_stub_ = Lease::NewStub(channel);
  lease_thread_.reset(nullptr);
  watch_thread_.reset(nullptr);
}	

Client::~Client() {
}

// TODO: timeout and retry
bool Client::Set(const std::string& key, const std::string& value, int64_t lease_id) {
  PutRequest req;
  PutResponse resp;
  req.set_key(key);
  req.set_value(value);
  // No need prev_kv in current interface
  // req.set_prev_kv(true);
  req.set_lease(lease_id);

  ClientContext context;
  Status status = kv_stub_.get()->Put(&context, req, &resp);

  if (status.ok()) {
    return true;
  }

  std::cerr << "Set Status: " << status.error_message() << std::endl;

  return false;
}

bool Client::Delete(const std::string& key) {
  DeleteRangeRequest req;
  DeleteRangeResponse resp;
  req.set_key(key);
  // No need prev_key in current interface
  // req.set_prev_kv(true);
  ClientContext context;
  Status status = kv_stub_.get()->DeleteRange(&context, req, &resp);

  return status.ok();
}

std::string Client::Get(const std::string& key) {
  RangeRequest req;
  RangeResponse resp;
  req.set_key(key);
  ClientContext context;
  Status status = kv_stub_.get()->Range(&context, req, &resp);
  if (!status.ok()) {
    return std::string("");
  }
  for (const auto& kv: resp.kvs()) {
    if (key == kv.key()) {
      return kv.value();
    }
  }
  return std::string("");
}

int64_t Client::LeaseGrant(int64_t ttl) {
  LeaseGrantRequest req;
  LeaseGrantResponse resp;
  req.set_ttl(ttl);
  req.set_id(0);

  ClientContext context;
  Status status = lease_stub_.get()->LeaseGrant(&context, req, &resp);
  if (status.ok()) {
    return resp.id();
  }
  std::cerr << "LeaseGrant Status: " << status.error_message() << std::endl;
  return 0;
}

void Client::KeepAlive(int64_t lease_id) {
  lease_thread_.reset(new std::thread([=]() {
    LeaseKeepAliveRequest req;
    LeaseKeepAliveResponse resp;
    ClientContext context;
    lease_id_.store(lease_id);
    req.set_id(lease_id_);
    std::unique_ptr<ClientReaderWriter<LeaseKeepAliveRequest,LeaseKeepAliveResponse>> stream = lease_stub_.get()->LeaseKeepAlive(&context);
    stream->Write(req);
    while (1) {
      if (stream->Read(&resp)) {
        int64_t ttl = resp.ttl();
	if (ttl == 0) {
	  // TODO: Do we want to quit here?
	  std::cout << "Warning: KeepAlive TTL: " << ttl << std::endl;
	  break;
	}
	// Note: ttl is in seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(ttl*1000/2));
        req.set_id(lease_id_.load());
	stream->Write(req);
      } else {
        // TODO: Shall we exit here?
	std::cerr << "KeepAlive: Read failed" << std::endl;
	break;
      }
    }
    std::cout << "Exiting KeepAlive..." << std::endl;
  }));
}

void Client::WatchGuard(const std::string& key, const std::string& value, int64_t ttl) {
  watch_thread_.reset(new std::thread([=](){
    ClientContext context;
    std::unique_ptr<ClientReaderWriter<WatchRequest,WatchResponse>> stream = watch_stub_.get()->Watch(&context);
    WatchRequest req;
    WatchResponse resp;
    WatchCreateRequest create_req;
    create_req.set_key(key);
    create_req.set_prev_kv(false);
    req.mutable_create_request()->CopyFrom(create_req);
    stream->Write(req);
    while (1) {
      if (stream->Read(&resp)) {
        for (const auto &ev: resp.events()) {
	  std::cout << "WatchGuard: Read Event: " << ev.type() << std::endl;
	  if (ev.type() == ::mvccpb::Event::DELETE) {
	    std::cout << "Caught: DELETE" << std::endl;
	    // When DELETE found, re-register
	    int64_t lease_id(0);
	    while (((lease_id = LeaseGrant(ttl)) == 0)) {
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
	      std::cerr << "WatchGuard: LeaseGrant retry" << std::endl;
	    }
	    lease_id_.store(lease_id);

	    while(!Set(key, value, lease_id)) {
	      std::cerr << "WatchGuard: Set failed: key: " << key 
	      << " | value: " << value 
	      << " | lease_id: " << lease_id << std::endl;
	      std::cerr << "WatchGuard: Set retry..." << std::endl;
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
	    }
	  }
	}
      } else {
	std::cerr << "WatchGuard: Read failed" << std::endl;
	break;
      }
    }
  }));
}

bool Client::Register(const std::string& key, const std::string& value, int64_t ttl) {
  int64_t lease_id = LeaseGrant(ttl);
  if (lease_id == 0) {
    std::cerr << "LeaseGrant Failed" << std::endl;
    return false;
  }
  bool ret = Set(key, value, lease_id);
  if (!ret) {
    std::cerr << "Set Failed" << std::endl;
    return false;
  }

  KeepAlive(lease_id);
  WatchGuard(key, value, ttl);

  return true;
}

} // namespace etcd
