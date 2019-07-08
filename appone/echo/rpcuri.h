#ifndef ARCH_RPCURI_H__
#define ARCH_RPCURI_H__

#include <initializer_list>

#include <Ice/Ice.h>
#include <IceBox/IceBox.h>

// #include "base/common.h"
// #include "base/network.h"

namespace arch {

// TODO:
// 希望用一个普通字符串表示一个服务
// 更优雅一点形式:  tcp://host:port/server?timeout=1000

// 现在 Ice 的形式:
// -------------- proxy --------------
// Locator:tcp -h xx.xx.xx.xx -p 1234
// Grid/Locator:tcp -h xx.xx.xx.xx -p 1234
// Grid/Locator:tcp -h xx.xx.xx.xx -p 1234:tcp -h xx.xx.xx.xx -p 1234

// --Ice.Default.Locator="CacheGrid/Locator:tcp -h xx.xx.xx.xx -p 1234"

// class RpcUri {
// public:
//   enum ProtocolType { UDP, TCP };
//   std::string host;
//   int port;
//   ProtocolType type;
//   std::string grid;
//   std::string protocol;
//   std::string query;
//   RpcUri *backup;

//   RpcUri(grid)

//   static bool Parse(const std::string &uri, RpcUri *result);
// };

// TODO: 

// forward declare
inline Ice::CommunicatorPtr CreateChannel(const std::string &default_locator);
inline Ice::LocatorPrxPtr CreateObjectById(Ice::CommunicatorPtr &cp,
                                           const std::string &object_id);

inline Ice::LocatorPrxPtr CreateObject(Ice::CommunicatorPtr &cp,
                                       const std::string &uri) {
  std::string::size_type pos_splash = uri.find_first_of('/');
  if (pos_splash == std::string::npos)
    return CreateObjectById(cp, uri);

  std::string::size_type pos_2 = uri.find_first_of('|', pos_splash);
  if (pos_splash == std::string::npos)
    return 0;

  std::string default_locator = uri.substr(0, pos_2);
  std::string object_id = uri.substr(pos_2 + 1);
  cp = CreateChannel(default_locator);
  if (cp) {
    return CreateObjectById(cp, object_id);
  }
  return 0;
}

typedef std::initializer_list<std::pair<const char *, const char *>> sslist;

// 最后一个条目key 必须是 0
inline Ice::CommunicatorPtr BuildCommunicator(const sslist &properties) {
  Ice::PropertiesPtr prs = Ice::createProperties();
  for (auto i : properties) {
    prs->setProperty(i.first, i.second);
  }

  Ice::InitializationData id;
  id.properties = prs;
  return Ice::initialize(id);
}

inline Ice::CommunicatorPtr CreateChannel(const std::string &default_locator) {
  std::string::size_type pos = default_locator.find_first_of('/');
  assert(pos != std::string::npos);

  const std::string name = default_locator.substr(0, pos);

  return BuildCommunicator({
      {"Ice.Override.ConnectTimeout", "300"},
      {"IceGrid.InstanceName", name.c_str()},
      {"Ice.MessageSizeMax", "20480"},   // TODO: it's important
      {"Ice.Default.Host", "127.0.0.1"}, // xce::Network::local_ip().c_str()},
      {"Ice.Default.Locator", default_locator.c_str()},
      {0, 0} // 标记结束
  });
}

inline Ice::LocatorPrxPtr CreateObjectById(Ice::CommunicatorPtr &cp,
                                           const std::string &object_id) {
  // 创建一个 CommunicatorPtr
  if (!cp) {
    int argc = 0;
    char *argv[] = {0};
    cp = Ice::initialize(argc, argv);
  }

  Ice::ObjectPrxPtr base;
  try {
    base = cp->stringToProxy(object_id);
  } catch (Ice::Exception &) {
  }
  return Ice::LocatorPrxPtr();
}

} // namespace arch
#endif // XCE_ARCH_RPCURI_H__
