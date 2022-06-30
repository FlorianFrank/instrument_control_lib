
#Paths containing the XML configuration files
add_definitions(-DDEVICE_CONFIG_FILE="${CMAKE_CURRENT_SOURCE_DIR}/config_files/SupportedDevices.xml")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=x86-64 -static -static-libgcc -static-libstdc++")
#set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -march=x86-64 -static -static-libgcc -static-libstdc++")


option(INSTRUMENT_LIB_BUILD_STATIC "Build instrument lib as static library"     ON)

# Set new CMP0077 policy used to build pugixml
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# Set general CXX options
set(CMAKE_CXX_STANDARD 17)
add_definitions(-DCXX=1)

# Definitions for the platform independent abstraction layer.
option(PIL_COMMUNICATION    "Enable PIL Sockets"          ON)
option(PIL_THREADING        "Enable PIL Threads"          ON)
option(PIL_LOGGING          "Enable Logging support"      ON)
option(PIL_SHARED           "BUILD PIL as shared library" OFF)
option(PIL_STATIC           "BUILD PIL as static library" ON)
option(PIL_CXX              "Enable PIL C++ support"      ON)

# PugiXML options
option(BUILD_SHARED_LIBS    "Build pugiXML as shared lib" OFF)
set(PUGI_XML_VERSION 1.12)