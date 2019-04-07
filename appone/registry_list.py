import os, sys
sys.path.append('../python/python')

import Ice, IceGrid
'''
One/Query
IceGrid/Query
DemoIceGrid/Query
One/Locator
IceGrid/Locator
DemoIceGrid/Locator
One/Registry
IceGrid/Registry
DemoIceGrid/Registry
One/LocatorRegistry
IceGrid/LocatorRegistry
DemoIceGrid/LocatorRegistry
DemoIceGrid/InternalRegistry-Master
'''

with Ice.initialize(sys.argv) as communicator:
    obj = communicator.stringToProxy(
        "DemoIceGrid/Registry:default -h localhost -p 4061")

    # obj.getSessionTimeout()

    r = IceGrid.RegistryPrx.checkedCast(obj)
    admin_session = r.createAdminSession("admin", "")
    print('replica name:', admin_session.getReplicaName())

    admin = admin_session.getAdmin()
    print('app names:', admin.getAllApplicationNames())


