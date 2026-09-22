#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "OptiMA::optima" for configuration "Debug"
set_property(TARGET OptiMA::optima APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(OptiMA::optima PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/liboptima.so.1.0.0"
  IMPORTED_SONAME_DEBUG "liboptima.so.1.0.0"
  )

list(APPEND _cmake_import_check_targets OptiMA::optima )
list(APPEND _cmake_import_check_files_for_OptiMA::optima "${_IMPORT_PREFIX}/lib/liboptima.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
