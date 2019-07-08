#

../../cpp/bin/icegridregistry --Ice.Config=config/register.cfg &
# icebox only
# ../cpp/test/IceGrid/simple/build/macosx/shared/server --Ice.Config=config/server.cfg &

#
DYLD_LIBRARY_PATH=../cpp/test/IceBox/deployer/build/macosx/shared \
  ../../cpp/bin/icebox --Ice.Config=config/icebox.cfg &

# ../cpp/bin/icegridadmin --Ice.Config=config/client.cfg

python registry_list.py

# prod
# ../cpp/bin/icegridregistry --daemon --Ice.Config=config/register.cfg
