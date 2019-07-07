.PHONY: slice all

all: slice echo libecho.so echo_client

echo.cpp: echo.ice
	slice2cpp echo.ice

CXXFLAGS=-g -O -I/opt/Ice/include -std=c++11 -I. -DICE_CPP11_MAPPING -fPIC
LDFLAGS=-L/opt/Ice/lib -rpath /opt/Ice/lib


%.o: %.cpp
	$(CC) $(CXXFLAGS) $< -c

%.o: %.cc
	$(CC) $(CXXFLAGS) $< -c

echo: echoI.o echo.o async.o callback.o page.o
	clang -stdlib=libstdc++ -lc++ -std=c++11 ${LDFLAGS} -lIce++11 -o echo echo.o echoI.o 

echo_client: echo_client.o echo.o async.o callback.o page.o
	clang -stdlib=libstdc++ -lc++ -std=c++11 ${LDFLAGS} -lIce++11 -o echo_client echo.o echo_client.o 


libecho.so: echoI.o echo.o
	clang -stdlib=libstdc++ -lc++ -std=c++11 -shared ${LDFLAGS} -lIce++11 -o libecho.so echo.o echoI.o
