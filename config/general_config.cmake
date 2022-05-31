
# Definitions for the platform independent abstraction layer.
option(COMMUNICATION "Enable Sockets"  ON)
option(SHARED "BUILD shared library"   ON)
option(CXX "Enable C++ support"        ON)

# Set new CMP0077 policy used to build pugixml
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)