#add_subdirectory(${TinyXML_INCLUDE_DIR})

# message             (STATUS "Testing for LuaPlus")
# find_package        ( LuaPlus REQUIRED )
# include_directories ( ${LUAPLUS_INCLUDE_DIR} )

#message(STATUS "Testing for TinyXML")
#find_package(TinyXML REQUIRED)
#include_directories(${TinyXML_INCLUDE_DIR})
#add_subdirectory(${TinyXML_INCLUDE_DIR})

# 
# MESSAGE(STATUS "Testing for Boost")
# # Boost thread
# FIND_PACKAGE(Boost REQUIRED filesystem thread) # for path.extentension need 1.36.0, workaround is in ResourceLoader.h
# INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
# message( "Using BOOST: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION} ${Boost_LIBRARIES}, ${Boost_INCLUDE_DIR}" )
# add_definitions(-DBOOST_VERSION_MAJOR=${Boost_MAJOR_VERSION})
# add_definitions(-DBOOST_VERSION_MINOR=${Boost_MINOR_VERSION})
# 
# MESSAGE(STATUS "Testing for IOS")
# # IOS
# FIND_PACKAGE(OIS REQUIRED)
# INCLUDE_DIRECTORIES(${OIS_INCLUDE_DIR})
# 
# MESSAGE(STATUS "Testing for OGRE")
# # Ogre
# FIND_PACKAGE(Ogre REQUIRED)
# INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIR})
# 
# MESSAGE(STATUS "Testing for Cegui")
# # CEGUI
# FIND_PACKAGE(CEGUI REQUIRED)
# INCLUDE_DIRECTORIES(${CEGUI_INCLUDE_DIRS})
# 
# MESSAGE(STATUS "Testing for ETM")
# FIND_PACKAGE(ETM REQUIRED)
# INCLUDE_DIRECTORIES(${ETM_INCLUDE_DIR})

######
# Build options.
#

#OPTION(DIRECTX  "Use DirectX as plugin for Ogre (Win32)"  OFF)


######
# Easy plugins.cfg
# Automaticly sets the right vars depending on os
# 
# IF(UNIX)
# #Comment out DirectX
#   SET(DX \#)
#   SET(GL )
# ENDIF(UNIX)
# 
# IF(WIN32)
#   IF(DIRECTX)
#       #Comment out OpenGL
#       SET(DX )
#       SET(GL \#)
#   ELSE(DIRECTX)
#       #Comment out DirectX
#       SET(DX \#)
#       SET(GL )
#   ENDIF(DIRECTX)
# ENDIF(WIN32)
# 
# SET(OGRE_PLUGINS ${OGRE_PlUGINS_DIR})
# #Do the plugins.cfg conversion
# CONFIGURE_FILE(cmake/plugins.cfg.in
#   ${CMAKE_BINARY_DIR}/plugins.cfg)
# 
# IF(OGRE_PLUGINS)
#   message(STATUS "Found Ogre plugins: ${OGRE_PLUGINS}")
# ELSE(OGRE_PLUGINS)
#   message(STATUS "Ogre plugins folder not found, please edit plugins.cfg manually.")
# ENDIF(OGRE_PLUGINS)

#create executable