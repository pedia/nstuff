#include <iostream>
#include <memory>
#include <string>

#include <Ice/Ice.h>

#include "async.h"
#include "callback.h"
#include "echo.h"
#include "page.h"

namespace one {

class EchoI : public Echo {
public:
  void echoVoid(const ::Ice::Current &current) {}

  //
  void echoVoid2(const ::Ice::Current &current) {}

  //
  int echoInt(int i, const ::Ice::Current &current) { return i; }

  int echoInt2(int i, int &o, const ::Ice::Current &current) {
    o = i;
    return i;
  }

  std::string echoString(std::string i, const ::Ice::Current &current) {
    return i;
  }

  //
  Response echoStruct(Request i, const ::Ice::Current &current) {
    Response r;
    r.val = i.val;
    return r;
  }

  // raise Error
  void raise(const ::Ice::Current &current) { throw Error(); }
};

class BookI : public Book {
public:
};

}; // namespace one

int main(int argc, char *argv[]) {
  try {
    Ice::CommunicatorHolder ich(argc, argv);
    auto adapter = ich->createObjectAdapterWithEndpoints("OneEchoAdapter",
                                                         "default -p 10000");
    auto servant = std::make_shared<one::EchoI>();
    adapter->add(servant, Ice::stringToIdentity("OneEcho"));
    adapter->activate();

    ich->waitForShutdown();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}
