import os, sys
sys.path.append('../python/python')

import Ice, IceGrid

with Ice.initialize(sys.argv) as communicator:
    obj = communicator.stringToProxy(
        "DemoIceGrid/Registry:default -h localhost -p 4061")

    # obj.getSessionTimeout()

    r = IceGrid.RegistryPrx.checkedCast(obj)
    admin_session = r.createAdminSession("admin", "")
    print('replica name:', admin_session.getReplicaName())

    admin = admin_session.getAdmin()
    print('app names:', admin.getAllApplicationNames())


Ice.loadSlice('../cpp/test/IceGrid/simple/Test.ice')
# import 