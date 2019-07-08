.PHONY: slice all

all: slice echo libecho.so echo_client test

echo.cpp: echo.ice
	slice2cpp echo.ice

CXXFLAGS=-g -O -I/opt/Ice/include -std=c++11 -I. -DICE_CPP11_MAPPING -fPIC -I/Users/pedia/fa/trade/src/third_party/googletest/googletest/include

LDFLAGS=-L/opt/Ice/lib -rpath /opt/Ice/lib -L/Users/pedia/fa/trade/src/third_party/googletest/googletest/lib/.libs


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
