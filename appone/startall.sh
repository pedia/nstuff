#

../cpp/bin/icegridregistry --Ice.Config=config/register.cfg &
../cpp/test/IceGrid/simple/build/macosx/shared/server --Ice.Config=config/server.cfg &
DYLD_LIBRARY_PATH=../cpp/test/IceBox/configuration/build/macosx/shared ../cpp/bin/icebox --Ice.Config=config/icebox.cfg &

# ../cpp/bin/icegridadmin --Ice.Config=config/client.cfg

python registry_list.py

# prod
# ../cpp/bin/icegridregistry --daemon --Ice.Config=config/register.cfg
