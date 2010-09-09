# -------------------------------------------------------------
project(tt3d)


# -------------------------------------------------------------
enable_language(C)
enable_language(CXX)


# -------------------------------------------------------------
cmake_minimum_required(VERSION 2.6)
list (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/projects/cmake/modules/" "${CMAKE_ROOT}/Modules")
include ( init.cmake )
include ( projects/cmake/macros.cmake )

set ( TARGET_SERVER "${PROJECT_NAME}-server" )
set ( TARGET_CLIENT "${PROJECT_NAME}-client" )
set ( TARGET_COMMON "${PROJECT_NAME}-common" )

#-------------------------------------------------------------
# Dependency checks.
#-------------------------------------------------------------
#set( Boost_DEBUG ON )
#set ( Boost_DIR "~/src/boost_1_42_0" )
set ( BOOST_ROOT "~/src/boost_1_42_0" )
set ( Boost_ADDITIONAL_VERSIONS "1.40" "1.41" "1.42" "1.42.0" "1.43" "1.44" )
set ( BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib )
set ( Boost_USE_STATIC_LIBS OFF )
set ( Boost_USE_MULTITHREAD ON )
find_package        ( Lua51 REQUIRED )
find_package        ( Boost 1.39 REQUIRED COMPONENTS unit_test_framework system thread )
set ( Boost_LIBRARY_DIRS ${BOOST_ROOT}/stage/lib )
find_package        ( ProtocolBuffers REQUIRED )
#glog

# -------------------------------------------------------------
# Sources
# -------------------------------------------------------------

#Prepare ProtoBuf 
WRAP_PROTO( PROTO_SRCS "proto/meta.proto" "proto/hello.proto" "proto/general.proto" )
message( STATUS ${PROTO_SRCS} )

file (GLOB_RECURSE SERVER_SRC   RELATIVE ${CMAKE_SOURCE_DIR} src/server/*.cpp src/server/*.c src/server/*.hpp src/server/*.h )
file (GLOB_RECURSE CLIENT_SRC   RELATIVE ${CMAKE_SOURCE_DIR} src/client/*.cpp src/client/*.c src/client/*.hpp src/client/*.h )
file (GLOB_RECURSE COMMON_SRC   RELATIVE ${CMAKE_SOURCE_DIR} src/core/*.cpp src/core/*.c src/core/*.hpp src/core/*.h )


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
  ${CMAKE_SOURCE_DIR}/src/core
  ${CMAKE_SOURCE_DIR}/src/server

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
  ${TARGET_COMMON}
)
set ( CLIENT_LIBS
  ${TARGET_COMMON}
)

link_directories (
   ${CMAKE_SOURCE_DIR}/3rdparty/lib
   ${Boost_LIBRARY_DIRS}
)
#message( STATUS ${Boost_LIBRARIES} )
#message( STATUS ${Boost_LIBRARY_DIRS} )
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
add_library( ${TARGET_COMMON} STATIC ${PROTO_SRCS} ${COMMON_SRC} )
add_executable( ${TARGET_SERVER} ${SERVER_SRC} ${PROTO_SRCS} )
add_executable( ${TARGET_CLIENT} ${PROTO_SRCS} ${CLIENT_SRC} )

add_subdirectory( ${3RDPARTY} )
add_dependencies( ${TARGET_SERVER} ${TARGET_COMMON} )
add_dependencies( ${TARGET_CLIENT} ${TARGET_COMMON} ${PROTO_SRCS} )

target_link_libraries ( ${TARGET_CLIENT} ${CLIENT_LIBS} )
target_link_libraries ( ${TARGET_SERVER} ${SERVER_LIBS} )
target_link_libraries ( ${TARGET_COMMON} ${COMMON_LIBS} )



# -------------------------------------------------------------
# Tests
# -------------------------------------------------------------
message( STATUS "Configured in ${CMAKE_BUILD_TYPE}" )
set ( TESTS_SOURCES 
  src/test/TestMain.cpp
  src/test/TestThreadPool.cpp
  src/test/TestIOServicePool.cpp
  src/test/TestTimerMaster.cpp
  src/test/ListenTest.cpp
)
set ( TEST test_${CMAKE_PROJECT_NAME} )
add_executable ( ${TEST} ${TESTS_SOURCES}  ${PROTO_SRCS} )
target_link_libraries ( ${TEST} ${PROJECT} ${COMMON_LIBS} ${CLIENT_LIBS} ${SERVER_LIBS}  ${TARGET_COMMON} )
enable_testing ()
add_test ( ${TEST} ${TEST} )

# common
#TARGET_LINK_LIBRARIES(ott3d tinyxml ${Boost_LIBRARIES})
# rendering client specific
#TARGET_LINK_LIBRARIES(ott3d ${OGRE_LIBRARY} ${OIS_LIBRARIES} ${ETM_LIBRARY} )
#TARGET_LINK_LIBRARIES(ott3d ${CEGUI_LIBRARY} ${CEGUI_RENDERER_LIBRARY} )
