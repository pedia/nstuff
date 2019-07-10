#include "benchmark/benchmark.h"

#include <Ice/Ice.h>
#include <grpcpp/grpcpp.h>

#include "echo.h"
#include "grpc_bench.grpc.pb.h"

static void BM_EchoVoid(benchmark::State& state) {
  Ice::InitializationData data;
  Ice::CommunicatorHolder ich = Ice::initialize(data);
  auto base = ich->stringToProxy("OneEcho2:default -p 10000");
  auto echo = Ice::checkedCast<one::EchoPrx>(base);
  if (!echo) {
    throw "Invalid proxy";
  }

  for (auto _ : state)
    echo->echoVoid();
}
BENCHMARK(BM_EchoVoid);

static void BM_EchoVoid2(benchmark::State& state) {
  Ice::InitializationData data;
  Ice::CommunicatorHolder ich = Ice::initialize(data);
  auto base = ich->stringToProxy("OneEcho2:default -p 10000");
  auto echo = Ice::checkedCast<one::EchoPrx>(base);
  if (!echo) {
    throw "Invalid proxy";
  }

  for (auto _ : state)
    echo->echoVoid2();
}
BENCHMARK(BM_EchoVoid2);

static void BM_EchoInt(benchmark::State& state) {
  Ice::InitializationData data;
  Ice::CommunicatorHolder ich = Ice::initialize(data);
  auto base = ich->stringToProxy("OneEcho2:default -p 10000");
  auto echo = Ice::checkedCast<one::EchoPrx>(base);
  if (!echo) {
    throw "Invalid proxy";
  }

  for (auto _ : state)
    echo->echoInt(3);
}
BENCHMARK(BM_EchoInt);

///
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel) : stub_(grpc_bench::Echo::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  void SayHello() {
    // Data we are sending to the server.
    grpc_bench::Empty e, r;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->echoVoid(&context, e, &r);

    if (!status.ok()) {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    }
  }

  std::unique_ptr<grpc_bench::Echo::Stub> stub_;
};

static void BM_GrpcEcho(benchmark::State& state) {
  GreeterClient greeter(grpc::CreateChannel("127.0.0.1:20000", grpc::InsecureChannelCredentials()));

  for (auto _ : state)
    greeter.SayHello();
}
BENCHMARK(BM_GrpcEcho);

static void BM_GrpcEchoRaw(benchmark::State& state) {
  auto channel = grpc::CreateChannel("127.0.0.1:20000", grpc::InsecureChannelCredentials());
  auto stub = grpc_bench::Echo::NewStub(channel);
  grpc_bench::Empty e, r;

  for (auto _ : state) {
    ClientContext context;
    stub->echoVoid(&context, e, &r);
  }
}
BENCHMARK(BM_GrpcEchoRaw);

static void BM_GrpcEchoInt(benchmark::State& state) {
  auto channel = grpc::CreateChannel("127.0.0.1:20000", grpc::InsecureChannelCredentials());
  auto stub = grpc_bench::Echo::NewStub(channel);
  grpc_bench::OneInt e, r;

  for (auto _ : state) {
    ClientContext context;
    stub->echoInt(&context, e, &r);
  }
}
BENCHMARK(BM_GrpcEchoInt);

BENCHMARK_MAIN();
