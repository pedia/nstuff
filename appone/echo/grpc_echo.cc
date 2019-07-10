
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "grpc_bench.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

// Logic and data behind the server's behavior.
class EchoServiceImpl final : public grpc_bench::Echo::Service {
  Status echoVoid(ServerContext* context, const grpc_bench::Empty* request,
                  grpc_bench::Empty* reply) override {
    // std::string prefix("Hello ");
    // reply->set_message(prefix + request->name());
    return Status::OK;
  }
  Status echoInt(ServerContext* context, const grpc_bench::OneInt* request,
                  grpc_bench::OneInt* reply) override {
    // std::string prefix("Hello ");
    reply->set_a(3);
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:20000");
  EchoServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
