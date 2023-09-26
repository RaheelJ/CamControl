
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was libONVIFConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

set_and_check(CMAKE_INSTALL_DIR "${PACKAGE_PREFIX_DIR}/cmake")
set_and_check(BIN_INSTALL_DIR "${PACKAGE_PREFIX_DIR}/bin")
set_and_check(INCLUDE_INSTALL_DIR "${PACKAGE_PREFIX_DIR}/include")
set_and_check(LIB_INSTALL_DIR "${PACKAGE_PREFIX_DIR}/lib")

include("${CMAKE_INSTALL_DIR}/libONVIFTargets.cmake")
include_directories("${INCLUDE_INSTALL_DIR}")

set(ENV{PATH} "${BIN_INSTALL_DIR};$ENV{PATH}")
