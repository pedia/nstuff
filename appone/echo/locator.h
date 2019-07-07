#ifndef ONE_LOCATOR_H__
#define ONE_LOCATOR_H__

#include <Ice/Ice.h>
#include <IceGrid/Query.h>

// #include "base/common.h"
// #include "base/network.h"
// #include "base/asyncall.h"
// #include "base/logging.h"

// #include "arch/rpcuri.h"

namespace arch {

enum InvokeType { ONE_WAY, TWO_WAY, BATCH_ONE_WAY, DATAGRAM };

#if 0
switch (it) {
case TWO_WAY:
  return prx;
case ONE_WAY:
  return prx->ice_oneway();
case BATCH_ONE_WAY:
  return prx->ice_batchOneway();
case DATAGRAM:
  return prx->ice_datagram();
default:
  return prx;
}
#endif

template <InvokeType E> struct ProxyType {
  Ice::ObjectPrx operator()(const Ice::ObjectPrx &prx) const { return prx; }
};

template <> struct ProxyType<ONE_WAY> {
  Ice::ObjectPrx operator()(const Ice::ObjectPrx &prx) const {
    return prx->ice_oneway();
  }
};

inline Ice::CommunicatorPtr CloneCommunicator(const Ice::CommunicatorPtr &c) {
  Ice::PropertiesPtr pp = c->getProperties();
  Ice::InitializationData id;
  id.properties = pp;
  return Ice::initialize(id);
}

// Locator: 封装 Communicator 及 Prx 的创建
// concept:
//   Prx Create(std::string const& proxy_or_id)
//   Prx CreateUnchecked(std::string const& proxy_or_id)

// 支持
// BoxLocator: 可执行的box, 形如 TheShop:default -h 10.2.18.186 -p 10000
// ReplicateLocator 从 registry 得到服务地址,原来的OceChannel

// TODO: 缓存 registery 结果，Prx失效后，减少重查 registery 的请求的量

class BoxLocator {
public:
  BoxLocator() : communicator_(Ice::initialize()) {}
  BoxLocator(const Ice::CommunicatorPtr &cp) : communicator_(cp) {}

  // proxy 形如 TheShop:default -h 10.2.18.186 -p 10000
  template <typename Prx, InvokeType Invoke>
  Prx Create(std::string const &proxy) {
    Ice::ObjectPrx base = communicator_->stringToProxy(proxy);
    return Prx::checkedCast(ProxyType<Invoke>()(base));
  }

  template <typename Prx, InvokeType Invoke>
  Prx CreateUnchecked(std::string const &proxy) {
    Ice::ObjectPrx base = communicator_->stringToProxy(proxy);
    return Prx::uncheckedCast(ProxyType<Invoke>()(base));
  }

  static Ice::CommunicatorPtr Default() {
    const stringpair arr[] = {
        {"Ice.Override.ConnectTimeout", "300"},
        {"Ice.ThreadPool.Client.Size", "30"},
        {"Ice.ThreadPool.Server.Size", "30"},
        {"Ice.MessageSizeMax", "20480"},
        {"Ice.Default.Host", Network::local_ip().c_str()},
        {0, 0} // 标记结束
    };
    return BuildCommunicator(arr);
  }

  static BoxLocator &Instance() {
    // TODO: use boost::once
    static BoxLocator inst_(Default());
    return inst_;
  }

private:
  Ice::CommunicatorPtr communicator_;
};

// 1 Communicator 必须cache，也需要提供自由创建的能力
// 2 是否应该以DefaultLocator为核心呢？ Ice::LocatorPrx
class RegistryLocator {
public:
  RegistryLocator() {}
  RegistryLocator(const Ice::CommunicatorPtr &cp) : communicator_(cp) {
    Ice::PropertiesPtr pp = cp->getProperties();
    std::string instance_name = pp->getProperty("IceGrid.InstanceName");
    assert(!instance_name.empty());

    query_ = IceGrid::QueryPrx::uncheckedCast(
        cp->stringToProxy(instance_name + "/Query"));
  }

  Ice::CommunicatorPtr communicator() const { return communicator_; }

  void SetProperty(const char *key, const char *value) {
    ASSERT(key && value);
    Ice::PropertiesPtr pp = communicator()->getProperties();
    if (pp)
      pp->setProperty(key, value);
  }

  template <class Prx, InvokeType Invoke>
  Prx Create(std::string const &proxy) const {
    std::string str = proxy;

    Ice::ObjectPrx base = communicator()->stringToProxy(proxy);
    return Prx::checkedCast((base));
  }

  template <class Prx, InvokeType Invoke>
  Prx CreateUnchecked(std::string const &proxy) const {
    Ice::ObjectPrx base = communicator()->stringToProxy(proxy);
    return Prx::uncheckedCast(ProxyType<Invoke>()(base));
  }

  std::vector<Ice::ObjectPrx> FindAllReplicas(const Ice::ObjectPrx &proxy) {
    return query_->findAllReplicas(proxy);
  }
  std::vector<Ice::ObjectPrx> FindAllObjectType(const std::string &type) {
    return query_->findAllObjectsByType(type);
  }

  // 受宏控制的
  static Ice::CommunicatorPtr Default() {
#if defined(XCE_LOCATOR_CUSTOM)
    const xce::stringpair arr[] = {
        {"Ice.Override.ConnectTimeout", "300"},
        {"Ice.ThreadPool.Client.StackSize", "65535"},
        // {"Ice.ThreadPool.Client.Size", "1"},
        // {"Ice.ThreadPool.Client.SizeMax", "1"},
        {"Ice.ThreadPool.Server.StackSize", "65535"},
        {"Ice.MessageSizeMax", "20480"},
        {"Ice.Default.Host", xce::Network::local_ip().c_str()},
#ifdef XCE_INSTANCE_NAME
        {"IceGrid.InstanceName", XCE_INSTANCE_NAME}, // default use IceGrid
#endif
        {"Ice.Default.Locator",
         XCE_DEFAULT_LOCATOR}, // "IceGrid/Locator:tcp -h 10.2.18.202 -p 4061"
        // {"Ice.Trace.Network", "1"},
        // {"Ice.Trace.Protocol", "1"},
        {0, 0} // 标记结束
    };
    return xce::BuildCommunicator(arr);
#endif

    return xiaonei();
  }

  static Ice::CommunicatorPtr xiaonei() {
    const stringpair arr[] = {
        {"Ice.Override.ConnectTimeout", "300"},
        {"IceGrid.InstanceName", "XiaoNei"},
        {"Ice.ThreadPool.Client.StackSize", "65535"},
        {"Ice.ThreadPool.Server.StackSize", "65535"},
        {"Ice.MessageSizeMax", "20480"},
        {"Ice.Default.Host", Network::local_ip().c_str()},
        // {"Ice.Default.Host", IpAddr::inner()},

        {"Ice.Default.Locator",
         "XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h "
         "XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 "
         "-p 12002"},
        // {"Ice.Default.Locator", "IceGrid/Locator:tcp -h 10.2.18.202 -p
        // 4061"},
        // {"Ice.Trace.Network", "1"},
        // {"Ice.Trace.Protocol", "1"},
        {0, 0} // 标记结束
    };
    return BuildCommunicator(arr);
  }
  static Ice::CommunicatorPtr cached_xiaonei() {
    static Ice::CommunicatorPtr ic_ = xiaonei(); // unsafe singleton, even leak
    return ic_;
  }

  static Ice::CommunicatorPtr passport() {
    const stringpair arr[] = {
        {"Ice.Override.ConnectTimeout", "300"},
        {"IceGrid.InstanceName", "Passport"},
        {"Ice.ThreadPool.Client.StackSize", "65535"},
        {"Ice.ThreadPool.Server.StackSize", "65535"},
        {"Ice.MessageSizeMax", "20480"},
        {"Ice.Default.Host", Network::local_ip().c_str()},
        {"Ice.Default.Locator",
         "Passport/Locator:default -h PassportMaster -p 4076:default -h "
         "PassportReplica1 -p 4076:default -h PassportReplica2 -p 4076"},
        {0, 0} // 标记结束
    };
    return BuildCommunicator(arr);
  }

  static Ice::CommunicatorPtr search_cache() {
    const stringpair arr[] = {
        {"Ice.Override.ConnectTimeout", "300"},
        {"IceGrid.InstanceName", "XiaoNei"},
        {"Ice.ThreadPool.Client.StackSize", "65535"},
        {"Ice.ThreadPool.Server.StackSize", "65535"},
        {"Ice.MessageSizeMax", "20480"},
        {"Ice.Default.Host", Network::local_ip().c_str()},
        {"Ice.Default.Locator",
         "SearchCache/Locator:default -h 10.2.17.61 -p 4061"},
        {0, 0} // 标记结束
    };
    return BuildCommunicator(arr);
  }

  static Ice::CommunicatorPtr talk() {
    const stringpair arr[] = {
        {"Ice.Override.ConnectTimeout", "300"},
        {"IceGrid.InstanceName", "XiaoNei"},
        {"Ice.ThreadPool.Client.StackSize", "65535"},
        {"Ice.ThreadPool.Server.StackSize", "65535"},
        {"Ice.MessageSizeMax", "20480"},
        {"Ice.Default.Host", Network::local_ip().c_str()},
        {"Ice.Default.Locator",
         "XNTalk/Locator:default -h XNTalkRegistry -p 12000"},
        {0, 0} // 标记结束
    };
    return BuildCommunicator(arr);
  }

  static Ice::CommunicatorPtr feed() {
    const stringpair arr[] = {
        {"Ice.Override.ConnectTimeout", "300"},
        {"IceGrid.InstanceName", "XceFeed"},
        {"Ice.ThreadPool.Client.StackSize", "65535"},
        {"Ice.ThreadPool.Server.StackSize", "65535"},
        {"Ice.MessageSizeMax", "20480"},
        {"Ice.Default.Host", Network::local_ip().c_str()},
        {"Ice.Default.Locator",
         "XceFeed/Locator:default -h XceFeedRegistry -p 14300:default -h "
         "XceFeedRegistryReplica1 -p 14301:default -h XceFeedRegistryReplica2 "
         "-p 14302"},
        {0, 0} // 标记结束
    };
    return BuildCommunicator(arr);
  }

#if 0  // TODO: IceGrid.InstanceName
  static Ice::CommunicatorPtr search() {
    const stringpair arr [] = {
      {"Ice.Override.ConnectTimeout", "300"},
      {"IceGrid.InstanceName", ""},
      {"Ice.ThreadPool.Client.StackSize", "65535"},
      {"Ice.ThreadPool.Server.StackSize", "65535"},
      {"Ice.MessageSizeMax", "20480"},
      {"Ice.Default.Host", Network::local_ip().c_str()},
      {"Ice.Default.Locator", "XiaoNeiSearch/Locator:default -h XiaoNeiSearchRegistry -p 4061:default -h XiaoNeiSearchRegistryReplica1 -p 4061:default -h XiaoNeiSearchRegistryReplica2 -p 4061"},
      {0, 0} // 标记结束
    };
    return BuildCommunicator(arr);
  }

  static Ice::CommunicatorPtr follow() {
    const stringpair arr [] = {
      {"Ice.Override.ConnectTimeout", "300"},
      {"IceGrid.InstanceName", "XiaoNei"},
      {"Ice.ThreadPool.Client.StackSize", "65535"},
      {"Ice.ThreadPool.Server.StackSize", "65535"},
      {"Ice.MessageSizeMax", "20480"},
      {"Ice.Default.Host", Network::local_ip().c_str()},
      {"Ice.Default.Locator", "Follow/Locator:default -h FollowRegistry -p 15300:default -h FollowRegistryReplica1 -p 15301:default -h FollowRegistryReplica2 -p 15302"},
      {0, 0} // 标记结束
    };
    return BuildCommunicator(arr);
  }

  static Ice::CommunicatorPtr log() {
    const stringpair arr [] = {
      {"Ice.Override.ConnectTimeout", "300"},
      {"IceGrid.InstanceName", "XiaoNei"},
      {"Ice.ThreadPool.Client.StackSize", "65535"},
      {"Ice.ThreadPool.Server.StackSize", "65535"},
      {"Ice.MessageSizeMax", "20480"},
      {"Ice.Default.Host", Network::local_ip().c_str()},
      {"Ice.Default.Locator", "XceLog/Locator:default -h LogMaster -p 4062:default -h LogReplica1 -p 4062:default -h LogReplica2 -p 4062"},
      {0, 0} // 标记结束
    };
    return BuildCommunicator(arr);
  }

  static Ice::CommunicatorPtr active_track() {
    const stringpair arr [] = {
      {"Ice.Override.ConnectTimeout", "300"},
      {"IceGrid.InstanceName", "XiaoNei"},
      {"Ice.ThreadPool.Client.StackSize", "65535"},
      {"Ice.ThreadPool.Server.StackSize", "65535"},
      {"Ice.MessageSizeMax", "20480"},
      {"Ice.Default.Host", Network::local_ip().c_str()},
      {"Ice.Default.Locator", "ActiveTrack/Locator:default -h ActiveTrackMaster -p 4065:default -h ActiveTrackReplica1 -p 4065:default -h ActiveTrackReplica2 -p 4065"},
      {0, 0} // 标记结束
    };
    return BuildCommunicator(arr);
  }
#endif // #if 0
  // TODO:
  // wservice
  // wService/Locator:default -h wServiceMaster -p 4064:default -h
  // wServiceReplica1 -p 4064:default -h wServiceReplica2 -p 4064

  // user monitor
  // UserMonitor/Locator:default -h UserMonitorMaster -p 4066:default -h
  // UserMonitorReplica1 -p 4066:default -h UserMonitorReplica2 -p 4066

private:
  Ice::CommunicatorPtr communicator_;
  IceGrid::QueryPrx query_;
};

#if 0
class ReplRegistryLocator : public RegistryLocator {
public:
  ReplRegistryLocator() {}
  ReplRegistryLocator(const Ice::CommunicatorPtr& cp)
    : RegistryLocator::RegistryLocator(cp) {
    Ice::PropertiesPtr pp = cp->getProperties();
    std::string instance_name = pp->getProperty("IceGrid.InstanceName");
    if (instance_name.empty())
      instance_name = "XiaoNei";
    query_ = IceGrid::QueryPrx::uncheckedCast(cp->stringToProxy(instance_name +
"/Query"));
  }
  std::vector<Ice::ObjectPrx> FindAllReplicas(const Ice::ObjectPrx &proxy) {
    return query_->findAllReplicas(proxy);
  }
private:
  IceGrid::QueryPrx query_;
};
#endif

//
template <typename Prx, class Locator = RegistryLocator,
          InvokeType Invoke = TWO_WAY>
class ClientPtr {
public:
  typedef Prx proxy_type;
  ClientPtr() {}

  ClientPtr(Prx p) : t_(p) {}

  ClientPtr(const Locator &locater, const std::string &proxy_or_id)
      : locator_(locater) {
    Create(proxy_or_id);
  }

  template <class Policy>
  ClientPtr(const Locator &locater, const Policy &pol, size_t uid)
      : locator_(locater) {
    Create(pol, uid);
  }

  ClientPtr(const Locator &locater) : locator_(locater) {}

  bool Create(std::string const &id) {
    t_ = locator_.template Create<Prx, Invoke>(id);
    return true;
  }
  bool CreateUnchecked(std::string const &id) {
    t_ = locator_.template CreateUnchecked<Prx, Invoke>(id);
    return true;
  }

  template <class Policy> bool Create(const Policy &pol, size_t uid) {
    // TODO: not thread safe
    static typename Policy::template CacheHolder<Prx, Locator, Invoke> cache_(
        (locator_), pol);
    t_ = cache_(uid);
    return true;
  }

  // for bool ! , never throw
  operator bool() const { return t_ != 0; }
  bool operator!() const { return t_ == 0; }

  Prx &operator->() throw() {
    // TODO: ASSERT
    return t_;
  }

  // TODO:
  void set_timeout(int millisec) { t_ = t_->ice_timeout(millisec); }

protected:
  Prx t_;
  Locator locator_;
};

template <typename Prx, typename Policy, InvokeType Invoke = TWO_WAY>
class ReplClientPtr : public ClientPtr<Prx, RegistryLocator, Invoke> {
public:
  typedef ClientPtr<Prx, RegistryLocator, Invoke> baseclass;

  ReplClientPtr(const Policy &pol,
                const RegistryLocator &locator = RegistryLocator::instance())
      : baseclass::ClientPtr(locator), cache_(locator, pol) {
    // Create(pol, uid);
    xce::Post(boost::bind(&ReplClientPtr::Run, this), 1000);
  }

  //
  ReplClientPtr &Locate(size_t uid) {
    baseclass::t_ = cache_(uid);
#if 0 // debug
    std::string adaid = t_->ice_getAdapterId();
    ::Ice::EndpointSeq ends = t_->ice_getEndpoints();
    std::string str = t_->ice_toString();
#endif
    return *this;
  }
  typename Policy::template CacheHolder<Prx, RegistryLocator, Invoke> cache_;

  std::vector<Prx> LocateAll(/*std::vector<Prx>* holder*/) {
    //     if (!holder)
    //       return std::vector<Prx>();
    std::vector<Prx> results;
    std::vector<Ice::ObjectPrx> proxies = SelectAllProxy(baseclass::t_);
    // ClientPtr<Prx, ReplRegistryLocator, Invoke>::Create(name, id));
    for (std::vector<Ice::ObjectPrx>::iterator it = proxies.begin();
         it != proxies.end(); ++it) {
      results.push_back(Prx::checkedCast(*it));
    }
    return results;
  }

  Prx &operator->() throw() {
    // TODO: ASSERT
    // static Prx t;
    t_ = Prx::uncheckedCast(SelectBestProxy(baseclass::t_));
    return t_;
  }

private:
  void EnsureReplicatedProxyInCache(const Ice::ObjectPrx &replicated) {
    ReplicatedProxyCache::iterator it =
        _replicated_proxies_cache.find(replicated);
    if (it != _replicated_proxies_cache.end())
      return;
    std::vector<Ice::ObjectPrx> x =
        baseclass::locator_.FindAllReplicas(replicated);
    _replicated_proxies_cache[replicated] = make_pair(ProxySeq(), x);
  }

  Ice::ObjectPrx SelectBestProxy(const Ice::ObjectPrx &replicated) {
    IceUtil::Mutex::Lock lock(_replicated_proxies_mutex);
    EnsureReplicatedProxyInCache(replicated);
    ReplicatedProxyCache::iterator it =
        _replicated_proxies_cache.find(replicated);
    if (it == _replicated_proxies_cache.end())
      return 0;
    int valid_size = it->second.first.size();
    int invalid_size = it->second.second.size();
    LOG(WARNING) << "Adapter::select_best_proxy valid: " << valid_size
                 << " invalid: " << invalid_size;
    if (valid_size > 0) {
      return it->second.first.at(rand() % valid_size);
    } else if (invalid_size > 0) {
      Ice::ObjectPrx prx = it->second.second.at(rand() % invalid_size);

#if 0 // debug
      std::string adaid = prx->ice_getAdapterId();
      ::Ice::EndpointSeq ends = prx->ice_getEndpoints();
      std::string str = prx->ice_toString();
#endif
      return prx;
    } else {
      return replicated;
    }
  }

  std::vector<Ice::ObjectPrx> SelectAllProxy(const Ice::ObjectPrx &replicated) {
    std::vector<Ice::ObjectPrx> results;
    IceUtil::Mutex::Lock lock(_replicated_proxies_mutex);
    EnsureReplicatedProxyInCache(replicated);
    ReplicatedProxyCache::iterator it =
        _replicated_proxies_cache.find(replicated);
    if (it == _replicated_proxies_cache.end()) {
      LOG(WARNING) << ("Adapter::select_all_proxy returns empty");
      return results;
    }
    LOG(WARNING) << "Adapter::select_all_proxy valid: "
                 << it->second.first.size();
    LOG(WARNING) << "Adapter::select_all_proxy invalid: "
                 << it->second.second.size();
    for (std::vector<Ice::ObjectPrx>::iterator itr = it->second.first.begin();
         itr != it->second.first.end(); ++itr) {
      results.push_back(*itr);
    }
    for (std::vector<Ice::ObjectPrx>::iterator itr = it->second.second.begin();
         itr != it->second.second.end(); ++itr) {
      results.push_back(*itr);
    }
    LOG(WARNING) << "Adapter::select_all_proxy returns " << results.size();
    return results;
  }

  bool isValid(const Ice::ObjectPrx &proxy) {
    return Prx::uncheckedCast(proxy)->isValid();
  }

  void Run() {
    try {
      ProxySeq keys;
      {
        IceUtil::Mutex::Lock lock(_replicated_proxies_mutex);
        for (ReplicatedProxyCache::const_iterator it =
                 _replicated_proxies_cache.begin();
             it != _replicated_proxies_cache.end(); ++it) {
          keys.push_back(it->first);
        }
      }
      ReplicatedProxyCache new_cache;
      for (ProxySeq::const_iterator it = keys.begin(); it != keys.end(); ++it) {
        ProxySeqPair validate_result;
        ProxySeq all_replicas = baseclass::locator_.FindAllReplicas(*it);
        // locator_.template Create<Prx, Invoke>
        for (ProxySeq::size_type x = 0; x < all_replicas.size(); ++x) {
          try {
            if (isValid(all_replicas[x]->ice_twoway())) {
              validate_result.first.push_back(all_replicas[x]);
            } else {
              validate_result.second.push_back(all_replicas[x]);
            }
          } catch (Ice::Exception &e) {
            // ensure timeout exception will not break whole thing.
            LOG(WARNING)
                << "Adapter::ValidateThread::run() Ice::Ex while validating "
                << e;
          } catch (...) {
            // ensure timeout exception will not break whole thing.
            LOG(WARNING) << "Adapter::ValidateThread::run() some exception "
                            "while validating "
                         << *it;
          }
        }
        new_cache.insert(make_pair(*it, validate_result));
      }

      {
        IceUtil::Mutex::Lock lock(_replicated_proxies_mutex);
        for (ReplicatedProxyCache::iterator it =
                 _replicated_proxies_cache.begin();
             it != _replicated_proxies_cache.end(); ++it) {
          ReplicatedProxyCache::iterator new_it = new_cache.find(it->first);
          if (new_it == new_cache.end())
            continue;
          it->second.first.swap(new_it->second.first);
          it->second.second.swap(new_it->second.second);
          // MCE_TRACE("ReplicatedAdapterI::ValidateThread::run " << it->first
          // << " Befor:"<<new_it->second.first.size()<<" After:" <<
          // it->second.first.size() );
        }
      }
    } catch (...) {
      // Ensure this thread will not terminate.
      LOG(WARNING)
          << "Adapter::ValidateThread::run() some exception in main loop.";
    }
  }
  IceUtil::Mutex _replicated_proxies_mutex;
  typedef std::vector<Ice::ObjectPrx> ProxySeq;
  typedef std::pair<ProxySeq, ProxySeq> ProxySeqPair;
  typedef std::map<Ice::ObjectPrx, ProxySeqPair> ReplicatedProxyCache;
  ReplicatedProxyCache _replicated_proxies_cache;
  Prx t_;
};

template <typename Prx, typename Policy, InvokeType Invoke = TWO_WAY>
class NewClientPtr : public ClientPtr<Prx, RegistryLocator, Invoke> {
public:
  typedef ClientPtr<Prx, RegistryLocator, Invoke> baseclass;

  NewClientPtr(const Policy &pol,
               const RegistryLocator &locator = RegistryLocator::instance())
      : baseclass::ClientPtr(locator), cache_(locator, pol) {
    // Create(pol, uid);
  }

  //
  NewClientPtr &Locate(size_t uid) {
    baseclass::t_ = cache_(uid);
#if 0 // debug
    std::string adaid = t_->ice_getAdapterId();
    ::Ice::EndpointSeq ends = t_->ice_getEndpoints();
    std::string str = t_->ice_toString();
#endif
    return *this;
  }
  typename Policy::template CacheHolder<Prx, RegistryLocator, Invoke> cache_;

  std::vector<Prx> LocateAll(/*std::vector<Prx>* holder*/) {

    std::vector<Prx> results;
    // std::vector<Ice::ObjectPrx> proxies = SelectAllProxy(baseclass::t_);
    ////ClientPtr<Prx, ReplRegistryLocator, Invoke>::Create(name, id));
    // for (std::vector<Ice::ObjectPrx>::iterator it = proxies.begin(); it !=
    // proxies.end(); ++it) {
    //  results.push_back(Prx::checkedCast(*it));
    //}
    return results;
  }

  /*Prx& operator->() throw() {
    return baseclass::t_;
  }*/

private:
  /*void EnsureReplicatedProxyInCache( const Ice::ObjectPrx& replicated) {
    ReplicatedProxyCache::iterator it = _replicated_proxies_cache .find(
      replicated);
    if (it != _replicated_proxies_cache.end())
      return;
    std::vector<Ice::ObjectPrx> x =
  baseclass::locator_.FindAllReplicas(replicated);
    _replicated_proxies_cache[replicated] = make_pair(ProxySeq(), x);
  }
  std::vector<Ice::ObjectPrx> SelectAllProxy(const Ice::ObjectPrx& replicated) {
    std::vector<Ice::ObjectPrx> results;
    IceUtil::Mutex::Lock lock(_replicated_proxies_mutex);
    EnsureReplicatedProxyInCache(replicated);
    ReplicatedProxyCache::iterator
  it=_replicated_proxies_cache.find(replicated);
    if(it==_replicated_proxies_cache.end()){
      LOG(WARNING) <<("Adapter::select_all_proxy returns empty");
      return results;
    }
    LOG(WARNING) <<"Adapter::select_all_proxy valid: " <<
  it->second.first.size(); LOG(WARNING) <<"Adapter::select_all_proxy invalid: "
  << it->second.second.size(); for(std::vector<Ice::ObjectPrx>::iterator
  itr=it->second.first.begin();itr!=it->second.first.end();++itr){
      results.push_back(*itr);
    }
    for(std::vector<Ice::ObjectPrx>::iterator
  itr=it->second.second.begin();itr!=it->second.second.end();++itr){
      results.push_back(*itr);
    }
    LOG(WARNING) <<"Adapter::select_all_proxy returns " << results.size();
    return results;
  }
  typedef std::vector<Ice::ObjectPrx> ProxySeq;
  typedef std::pair<ProxySeq, ProxySeq> ProxySeqPair;
  typedef std::map<Ice::ObjectPrx, ProxySeqPair> ReplicatedProxyCache;
  ReplicatedProxyCache _replicated_proxies_cache;*/
};

// TODO:
// concept
//
class ReplModuloPolicy {
public:
  ReplModuloPolicy(const std::string &name, const std::string &adapter,
                   size_t cluster_count)
      : name_(name), adapter_(adapter), cluster_count_(cluster_count) {}

  ReplModuloPolicy(const std::string &proxy, size_t cluster_count)
      : proxy_(proxy), cluster_count_(cluster_count) {}

  std::string operator()(size_t uid) const { return ProxyName(uid); }

  size_t cluster(size_t uid) const { return uid % cluster_count_; }
  size_t cluster_count() const { return cluster_count_; }

  template <typename Prx, class Locator, InvokeType Invoke> struct CacheHolder {
    CacheHolder(const Locator &locator, const ReplModuloPolicy &pol)
        : pol_(pol), locator_(locator) {
      Init();
    }

    bool Init() {
      cache_.resize(pol_.cluster_count());
      for (size_t i = 0; i < pol_.cluster_count(); ++i) {
        cache_[i] = locator_.template Create<Prx, Invoke>(pol_(i));
      }
      return true;
    }

    Prx operator()(size_t uid) const { return cache_[pol_.cluster(uid)]; }

    typedef std::vector<Prx> cache_type;
    cache_type cache_;
    const ReplModuloPolicy &pol_;
    const Locator locator_;
  };

private:
  std::string ProxyName(size_t uid) const {
    std::ostringstream stem;
    if (proxy_.empty())
      stem << name_ << cluster(uid);
    else
      stem << proxy_ << cluster(uid);
    return stem.str();
  }

private:
  const std::string name_;
  const std::string adapter_;
  const std::string proxy_;
  size_t cluster_count_;
};

class ModuloPolicy {
public:
  ModuloPolicy(const std::string &name, const std::string &adapter,
               size_t cluster_count)
      : name_(name), adapter_(adapter), cluster_count_(cluster_count) {}

  ModuloPolicy(const std::string &proxy, size_t cluster_count)
      : proxy_(proxy), cluster_count_(cluster_count) {}

  std::string operator()(size_t uid) const { return ProxyName(uid); }

  size_t cluster(size_t uid) const { return uid % cluster_count_; }
  size_t cluster_count() const { return cluster_count_; }

  template <typename Prx, class Locator, InvokeType Invoke> struct CacheHolder {
    CacheHolder(const Locator &locator, const ModuloPolicy &pol)
        : pol_(pol), locator_(locator) {
      Init();
    }

    bool Init() {
      cache_.resize(pol_.cluster_count());
      for (size_t i = 0; i < pol_.cluster_count(); ++i) {
        cache_[i] = locator_.template Create<Prx, Invoke>(pol_(i));
      }
      return true;
    }

    Prx operator()(size_t uid) const { return cache_[pol_.cluster(uid)]; }

    typedef std::vector<Prx> cache_type;
    cache_type cache_;
    const ModuloPolicy &pol_;
    const Locator locator_;
  };

private:
  std::string ProxyName(size_t uid) const {
    std::ostringstream stem;
    if (proxy_.empty())
      stem << name_ << "@" << adapter_ << cluster(uid);
    else
      stem << proxy_ << cluster(uid);
    return stem.str();
  }

private:
  const std::string name_;
  const std::string adapter_;
  const std::string proxy_;
  size_t cluster_count_;
};

class SimpleModulo {
public:
  SimpleModulo(const std::string &name, const std::string &adapter,
               size_t cluster_count)
      : name_(name), adapter_(adapter), cluster_count_(cluster_count) {}

  SimpleModulo(const std::string &proxy, size_t cluster_count)
      : proxy_(proxy), cluster_count_(cluster_count) {}

  std::string operator()(size_t uid) const { return ProxyName(uid); }

  size_t cluster(size_t uid) const { return uid % cluster_count_; }
  size_t cluster_count() const { return cluster_count_; }

  template <typename Prx, class Locator, InvokeType Invoke> struct CacheHolder {
    CacheHolder(const Locator &locator, const SimpleModulo &pol)
        : pol_(pol), locator_(locator) {
      Init();
    }

    bool Init() {
      cache_.resize(pol_.cluster_count());
      for (size_t i = 0; i < pol_.cluster_count(); ++i) {
        cache_[i] = locator_.template Create<Prx, Invoke>(pol_(i));
      }
      return true;
    }

    Prx operator()(size_t uid) const { return cache_[pol_.cluster(uid)]; }

    typedef std::vector<Prx> cache_type;
    cache_type cache_;
    const SimpleModulo &pol_;
    const Locator locator_;
  };

private:
  std::string ProxyName(size_t uid) const {
    std::ostringstream stem;
    if (proxy_.empty())
      stem << name_ << "@" << adapter_ << cluster(uid);
    else
      stem << proxy_;
    return stem.str();
  }

private:
  const std::string name_;
  const std::string adapter_;
  const std::string proxy_;
  size_t cluster_count_;
};

} // namespace arch
#endif // ONE_LOCATOR_H__
