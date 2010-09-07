# -------------------------------------------------------------
project(tt3d)


# -------------------------------------------------------------
enable_language(C)
enable_language(CXX)


# -------------------------------------------------------------
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/projects/cmake/modules/")
include ( init.cmake )
include ( projects/cmake/macros.cmake )

set ( TARGET_SERVER "tt3d-server" )
set ( TARGET_CLIENT "tt3d-client" )


#-------------------------------------------------------------
# Dependency checks.
#
set ( Boost_USE_STATIC_LIBS OFF )
set ( Boost_USE_MULTITHREAD ON )
find_package        ( Lua51 REQUIRED )
find_package        ( Boost COMPONENTS unit_test_framework system thread )
find_package        ( ProtocolBuffers REQUIRED )
#glog

# -------------------------------------------------------------
# Sources
# -------------------------------------------------------------

#Prepare ProtoBuf 
WRAP_PROTO( PROTO_SRCS proto/meta.proto )
message( STATUS ${PROTO_SRCS} )

file (GLOB_RECURSE SERVER_SRC   RELATIVE ${CMAKE_SOURCE_DIR} src/server/*.cpp src/server/*.c src/server/*.hpp src/server/*.h )
file (GLOB_RECURSE CLIENT_SRC   RELATIVE ${CMAKE_SOURCE_DIR} src/client/*.cpp src/client/*.c src/client/*.hpp src/client/*.h )



#set ( CMAKE_BUILD_TYPE Release )
set ( CMAKE_BUILD_TYPE "Debug" )



# -------------------------------------------------------------
# Source directories.
# -------------------------------------------------------------
set ( INCLUDE_DIRS
  3rdparty/include
  src/core
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/inc
  ${CMAKE_SOURCE_DIR}/src/core/network

  # for proto/meta.pb.h
  ${CMAKE_CURRENT_BINARY_DIR}

  ${LUA_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${PROTOBUF_INCLUDE_DIR}
)

#used by both
set ( COMMON_LIBS
  glog
  ${Boost_LIBRARIES}
  ${LUA_LIBRARIES}
  ${PROTOBUF_LIBRARY}
)

set ( SERVER_LIBS

)
set ( CLIENT_LIBS

)

link_directories (
   ${CMAKE_SOURCE_DIR}/3rdparty/lib
)

include_directories ( ${INCLUDE_DIRS} )

# Preprocessor
if ( Boost_FOUND )
  add_definitions ( -D_WITH_BOOST -DBOOST_VERSION_MAJOR=${Boost_MAJOR_VERSION} -DBOOST_VERSION_MINOR=${Boost_MINOR_VERSION} )
endif ( Boost_FOUND )

#CMAKE_C_FLAGS_DEBUG

if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  add_definitions ( -D_DEBUG )
endif ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

# -------------------------------------------------------------
# Targets
# -------------------------------------------------------------
add_executable( ${TARGET_SERVER} ${SERVER_SRC} ${PROTO_SRCS} )
add_executable( ${TARGET_CLIENT} ${PROTO_SRCS} ${CLIENT_SRC} )

add_subdirectory( ${3RDPARTY} )
add_dependencies( ${TARGET_SERVER} depends )
add_dependencies( ${TARGET_CLIENT} ${PROTO_SRCS} )

target_link_libraries ( ${TARGET_SERVER} ${COMMON_LIBS} ${SERVER_LIBS} )
target_link_libraries ( ${TARGET_CLIENT} ${COMMON_LIBS} ${CLIENT_LIBS} )



# -------------------------------------------------------------
# Tests
# -------------------------------------------------------------
message( STATUS "Configured in ${CMAKE_BUILD_TYPE}" )
set ( TESTS_SOURCES 
  src/test/TestMain.cpp
  src/core/thread/ThreadPool.hpp
  src/core/network/IOServicePool.cpp
  src/server/ServerListener.cpp 
  src/client/NetworkClient.cpp 
  src/core/network/Connection.cpp
  src/core/network/RawConnection.cpp
  src/core/TimerMaster.cpp
  src/test/TimerMasterTest.cpp
)
set ( TEST test_${CMAKE_PROJECT_NAME} )
add_executable ( ${TEST} ${TESTS_SOURCES}  ${PROTO_SRCS} )
target_link_libraries ( ${TEST} ${PROJECT} ${COMMON_LIBS} ${CLIENT_LIBS} ${SERVER_LIBS} )
enable_testing ()
add_test ( ${TEST} ${TEST} )

# common
#TARGET_LINK_LIBRARIES(ott3d tinyxml ${Boost_LIBRARIES})
# rendering client specific
#TARGET_LINK_LIBRARIES(ott3d ${OGRE_LIBRARY} ${OIS_LIBRARIES} ${ETM_LIBRARY} )
#TARGET_LINK_LIBRARIES(ott3d ${CEGUI_LIBRARY} ${CEGUI_RENDERER_LIBRARY} )
