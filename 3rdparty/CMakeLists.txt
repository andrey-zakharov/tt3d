# ------
# Used to prepare dependencies


# ------------------------------------------------------------
# LuaPlus
# ------------------------------------------------------------
# find_package ( LuaPlus )
# 
# 
# if ( NOT ${LUAPLUS_INCLUDE_DIR} )
#     tt3dbuild_git_clone_depends_data ( ${LUAPLUS_URL} ${PROJECT_SOURCE_DIR}/${LUAPLUS_HOME} )
#     find_package ( LuaPlus )
# endif ( NOT ${LUAPLUS_INCLUDE_DIR} )
# 
# if ( NOT LUAPLUS_LIB )
#   #add_custom_command ( OUTPUT ${PROJECT_SOURCE_DIR}/${LUAPLUS_HOME}/Src/LuaPlus/bin/libLuaPlus.so
#   #  
#   #)
#   #add_dependencies ( depends ${PROJECT_SOURCE_DIR}/${LUAPLUS_HOME}/Src/LuaPlus/bin/libLuaPlus.so )
#   
#   file(GLOB LUAPLUS_COMPILE_SRC
#     ${PROJECT_SOURCE_DIR}/${LUAPLUS_HOME}/Src/LuaPlus/*.cpp 
#     ${PROJECT_SOURCE_DIR}/${LUAPLUS_HOME}/Src/LuaPlus/*.h 
#     ${PROJECT_SOURCE_DIR}/${LUAPLUS_HOME}/Src/LuaPlus/*.c
#     ${PROJECT_SOURCE_DIR}/${LUAPLUS_HOME}/Src/LuaPlus/*.inl)
#     
#   add_library(LuaPlus SHARED ${LUAPLUS_COMPILE_SRC})
#   #execute_process( 
#   #  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/${LUAPLUS_HOME}
#   #  COMMAND sh ./CreateJamXcodeWorkspace.sh
#   #)
# 
# endif ( NOT LUAPLUS_LIB )




