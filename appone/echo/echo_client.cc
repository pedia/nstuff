#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <Ice/Ice.h>

#include "echo.h"
#include "page.h"
#include "callback.h"
#include "async.h"

void Dump(const Ice::CommunicatorPtr &c) {
  std::cout << "bool check:" << (bool)c << std::endl;
  Ice::PropertiesPtr ps = c->getProperties();
  std::vector<std::string> ss = ps->getCommandLineOptions();
  std::cout << "properties:" << std::endl;
  for (auto s : ss)
    std::cout << " " << s << std::endl;
}

void TestPage(const Ice::CommunicatorPtr &c) {
  auto base = c->stringToProxy("OnePage:default -p 10000");
  auto o = Ice::checkedCast<one::PagePrx>(base);
  if (!o) {
    throw "Invalid proxy";
  }

  auto topics = o->fetch(0, 5);
  std::cout << "\nPage.fetch: " << topics.size() << std::endl;
}

int main(int argc, char *argv[]) {
  try {
    Ice::CommunicatorHolder ich(argc, argv);
    Dump(ich.communicator());

    auto base = ich->stringToProxy("OneEcho2:default -p 10000");
    auto echo = Ice::checkedCast<one::EchoPrx>(base);
    if (!echo) {
      throw "Invalid proxy";
    }

    echo->echoVoid();

    echo->echoVoid2();

    std::cout << "\nechoInt: " << echo->echoInt(3) << std::endl;

    int o = 0;
    std::cout << "echoInt2: " << echo->echoInt2(4, o) << " out: " << o
              << std::endl;

    std::cout << "echoString: " << echo->echoString("hello") << std::endl;

    one::Request req;
    req.val = 42;
    std::cout << "echoStruct val: " << echo->echoStruct(req).val << std::endl;

    try {
      echo->raise();
    } catch (const one::Error &e) {
      std::cout << "raise ?, got: \"" << e.what() << "\"" << std::endl;
    } catch (const Ice::Exception &e) {
      std::cout << "raise 0, what: \"" << e.what() << "\"" << std::endl
                << "file:line: " << e.ice_file() << ":" << e.ice_line()
                << std::endl;
      std::cout << "stack trace: " << e.ice_stackTrace() << std::endl;
    } catch (const std::exception &e) {
      std::cout << "raise 1, got: " << e.what() << std::endl;
    }

    TestPage(ich.communicator());

  } catch (const std::exception &e) {
    std::cerr << "outside exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
