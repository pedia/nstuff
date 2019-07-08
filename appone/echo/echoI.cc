// #include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
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

class PageI : public Page {
public:
  Topics fetch(int offset, int limit, const ::Ice::Current &current) {
    std::vector<Topic> topics_;

    for (int i = 0; i < limit; i++) {
      auto t = Topic();
      t.id = offset + i;
      topics_.push_back(t);
    }
    return topics_;
  }
};

class CallbackI : public Callback {
  std::mutex mutex_;
  std::set<std::shared_ptr<SinkerPrx>> set_;

  void sinkAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto i : set_) {
      i->sink();
    }
  }

public:
  void listen(std::shared_ptr<SinkerPrx> o, const ::Ice::Current &current) {
    std::unique_lock<std::mutex> lock(mutex_);
    set_.insert(o);
  }

  void unlisten(std::shared_ptr<SinkerPrx> o, const ::Ice::Current &current) {
    std::unique_lock<std::mutex> lock(mutex_);
    size_t c = set_.erase(o);
    std::cout << "CallbackI erase " << c << std::endl;
  }

  void sinkLater(int seconds, const ::Ice::Current &current) {
    auto start = std::chrono::steady_clock::now();
    std::async(std::launch::async, [this] {
      //
      sinkAll();
    });
    std::cout << "CallbackI left sinkLater" << std::endl;
  }

  void sinkNow(const ::Ice::Current &current) {
    //
    sinkAll();
    std::cout << "CallbackI left sinkAll" << std::endl;
  }
};

}; // namespace one

int main(int argc, char *argv[]) {
  try {
    Ice::CommunicatorHolder ich(argc, argv);
    auto adapter = ich->createObjectAdapterWithEndpoints("OneEchoAdapter",
                                                         "default -p 10000");
    {
      auto servant = std::make_shared<one::EchoI>();
      adapter->add(servant, Ice::stringToIdentity("OneEcho"));
    }
    {
      adapter->add(std::make_shared<one::EchoI>(),
                   Ice::stringToIdentity("OneEcho2"));
    }
    {
      adapter->add(std::make_shared<one::PageI>(),
                   Ice::stringToIdentity("OnePage"));
    }
    {
      adapter->add(std::make_shared<one::CallbackI>(),
                   Ice::stringToIdentity("OneCallback"));
    }
    adapter->activate();

    ich->waitForShutdown();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}
