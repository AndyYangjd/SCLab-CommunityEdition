# This module defines
# SARibbon_FOUND, if false, do not try to link to SARibbon
# SARibbon_INCLUDE_DIRS, where to find the headers
# SARibbon_LIBRARY_DIRS, where to find the libs

message(STATUS "Reading ${CMAKE_CURRENT_LIST_FILE}")
set(SARibbon_Version 0.0.1)
message(STATUS "SARibbon configuration, version ${SARibbon_Version}")

get_filename_component(_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
get_filename_component(_ROOT ${_DIR}/.. ABSOLUTE)
set(SARibbon_INCLUDE_DIRS "${_ROOT}/include")
set(SARibbon_LIBRARY_DIRS "${_ROOT}/lib")
set(SARibbon_LIBRARIES "")

set(SARIBBON_LIBS "SARibbonBar")
foreach(_var ${SARIBBON_LIBS})
    find_library (_var_path ${_var}
        PATHS "${_ROOT}/lib")
    list(APPEND SARibbon_LIBRARIES ${_var_path})
endforeach(_var ${SARIBBON_LIBS})

find_library ( 
  PATHS "${_ROOT}/lib")

set(SARibbon_BINARY_DIRS "${_ROOT}/bin")
set(SARibbon_FOUND TRUE)

unset (_ROOT)
unset (_DIR)
