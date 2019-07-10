.PHONY: slice all

all: slice echo libecho.so echo_client test bench grpc_echo grpc_async_echo

echo.cpp: echo.ice
	slice2cpp echo.ice

CXXFLAGS=-g -O -I/opt/Ice/include -std=c++11 -I. -DICE_CPP11_MAPPING -fPIC -I/usr/local/include

LDFLAGS=-L/opt/Ice/lib -rpath /opt/Ice/lib -L/usr/local/lib


%.cpp: %.ice
	slice2cpp $<

%.o: %.cpp
	$(CC) $(CXXFLAGS) $< -c

%.o: %.cc
	$(CC) $(CXXFLAGS) $< -c

echo: echoI.o echo.o async.o callback.o page.o
	clang -stdlib=libstdc++ -lc++ -std=c++11 ${LDFLAGS} -lIce++11 -o echo echo.o echoI.o page.o callback.o async.o

echo_client: echo_client.o echo.o async.o callback.o page.o
	clang -stdlib=libstdc++ -lc++ -std=c++11 ${LDFLAGS} -lIce++11 -o echo_client echo.o page.o echo_client.o callback.o async.o

test: rpcuri_test.o rpcuri.h
	clang $(CXXFLAGS) -o test rpcuri_test.o -lc++ -std=c++11 ${LDFLAGS} -lIce++11 -lIceBox++11 -lgtest -lgtest_main

libecho.so: echoI.o echo.o
	clang -stdlib=libstdc++ -lc++ -std=c++11 -shared ${LDFLAGS} -lIce++11 -o libecho.so echo.o echoI.o page.o callback.o async.o

bench: bench.o echo.o grpc_bench.grpc.pb.o grpc_bench.pb.o
	clang -stdlib=libstdc++ -lc++ -std=c++11 ${LDFLAGS} -lIce++11 -o bench echo.o bench.o grpc_bench.pb.o grpc_bench.grpc.pb.o -lbenchmark -lgrpc++ -lgrpc -lprotobuf -lcares -lz

grpc_bench.grpc.pb.cc: grpc_bench.proto
	protoc --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` grpc_bench.proto

grpc_echo: grpc_echo.o
	clang -stdlib=libstdc++ -lc++ -std=c++11 ${LDFLAGS} -lIce++11 -o grpc_echo grpc_echo.o grpc_bench.pb.o grpc_bench.grpc.pb.o -lgrpc++ -lgrpc -lprotobuf -lgrpcpp_channelz -lcares -lz

grpc_async_echo: grpc_async_echo.o
	clang -stdlib=libstdc++ -lc++ -std=c++11 ${LDFLAGS} -lIce++11 -o grpc_async_echo grpc_async_echo.o grpc_bench.pb.o grpc_bench.grpc.pb.o -lgrpc++ -lgrpc -lprotobuf -lgrpcpp_channelz -lcares -lz
