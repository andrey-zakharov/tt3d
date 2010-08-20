

# Macro to download data on the depends target
macro ( tt3dbuild_download_depends_data _url _filename )
  find_package(Wget REQUIRED)
  add_custom_command(OUTPUT ${_filename}
                     COMMAND cmake -E echo "[build] Downloading ${_url} to ${_filename}..."
                     COMMAND ${WGET_EXECUTABLE} -q ${_url} -O ${_filename}
                     COMMAND cmake -E echo "[build] Done."
                     VERBATIM)
  # Create a legal target name, in case the target name has slashes in it
  string(REPLACE "/" "_" _dependsname download_data_${_filename})
  add_custom_target(${_dependsname} DEPENDS ${_filename})
  # Redeclaration of target is to workaround bug in 2.4.6
  add_custom_target(depends)
  add_dependencies(depends ${_dependsname})
endmacro( tt3dbuild_download_depends_data )

# Macro to git clone 
macro ( tt3dbuild_git_clone_depends_data _url _dir )
  find_program(GIT git)
  add_custom_command(OUTPUT ${_dir}
                     COMMAND cmake -E echo "[build] Downloading ${_url} to ${_dir}..."
                     COMMAND ${GIT} clone -q ${_url} ${_dir}
                     COMMAND cmake -E echo "[build] Done."
                     VERBATIM)
  # Create a legal target name, in case the target name has slashes in it
  string(REPLACE "/" "_" _dependsname download_data_${_dir})
  add_custom_target(${_dependsname} DEPENDS ${_dir})
  # Redeclaration of target is to workaround bug in 2.4.6
  add_custom_target(depends)
  add_dependencies(depends ${_dependsname})
endmacro( tt3dbuild_git_clone_depends_data )

