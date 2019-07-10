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

  int SayHelloAsync() {
    // Data we are sending to the server.
    grpc_bench::OneInt request;
    // request.set_name(user);

    // Container for the data we expect from the server.
    grpc_bench::OneInt reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    grpc::CompletionQueue cq;

    // Storage for the status of the RPC upon completion.
    Status status;

    // stub_->PrepareAsyncSayHello() creates an RPC object, returning
    // an instance to store in "call" but does not actually start the RPC
    // Because we are using the asynchronous API, we need to hold on to
    // the "call" instance in order to get updates on the ongoing RPC.
    std::unique_ptr<grpc::ClientAsyncResponseReader<grpc_bench::OneInt> > rpc(
        stub_->PrepareAsyncechoInt(&context, request, &cq));

    // StartCall initiates the RPC call
    rpc->StartCall();

    // Request that, upon completion of the RPC, "reply" be updated with the
    // server's response; "status" with the indication of whether the operation
    // was successful. Tag the request with the integer 1.
    rpc->Finish(&reply, &status, (void*)1);
    void* got_tag;
    bool ok = false;
    // Block until the next result is available in the completion queue "cq".
    // The return value of Next should always be checked. This return value
    // tells us whether there is any kind of event or the cq_ is shutting down.
    GPR_ASSERT(cq.Next(&got_tag, &ok));

    // Verify that the result from "cq" corresponds, by its tag, our previous
    // request.
    GPR_ASSERT(got_tag == (void*)1);
    // ... and that the request was completed successfully. Note that "ok"
    // corresponds solely to the request for updates introduced by Finish().
    GPR_ASSERT(ok);

    // Act upon the status of the actual RPC.
    if (status.ok()) {
      return reply.a();
    } else {
      return 0;
    }
  }

  std::unique_ptr<grpc_bench::Echo::Stub> stub_;
};

static void BM_GrpcEchoSSCS(benchmark::State& state) {
  GreeterClient greeter(grpc::CreateChannel("127.0.0.1:20000", grpc::InsecureChannelCredentials()));

  for (auto _ : state)
    greeter.SayHello();
}
BENCHMARK(BM_GrpcEchoSSCS);


static void BM_GrpcEchoSSCA(benchmark::State& state) {
  GreeterClient greeter(grpc::CreateChannel("127.0.0.1:20000", grpc::InsecureChannelCredentials()));

  for (auto _ : state)
    greeter.SayHelloAsync();
}
BENCHMARK(BM_GrpcEchoSSCA);

static void BM_GrpcEchoSACA(benchmark::State& state) {
  GreeterClient greeter(grpc::CreateChannel("127.0.0.1:20001", grpc::InsecureChannelCredentials()));

  for (auto _ : state)
    greeter.SayHelloAsync();
}
BENCHMARK(BM_GrpcEchoSACA);

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

static void BM_GrpcEchoIntSACS(benchmark::State& state) {
  auto channel = grpc::CreateChannel("127.0.0.1:20001", grpc::InsecureChannelCredentials());
  auto stub = grpc_bench::Echo::NewStub(channel);
  grpc_bench::OneInt e, r;

  for (auto _ : state) {
    ClientContext context;
    stub->echoInt(&context, e, &r);
  }
}
BENCHMARK(BM_GrpcEchoIntSACS);



BENCHMARK_MAIN();
