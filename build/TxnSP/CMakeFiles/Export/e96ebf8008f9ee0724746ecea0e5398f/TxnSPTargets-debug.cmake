#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "TxnSP::txnsp" for configuration "Debug"
set_property(TARGET TxnSP::txnsp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(TxnSP::txnsp PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libtxnsp.so.1.0.0"
  IMPORTED_SONAME_DEBUG "libtxnsp.so.1.0.0"
  )

list(APPEND _cmake_import_check_targets TxnSP::txnsp )
list(APPEND _cmake_import_check_files_for_TxnSP::txnsp "${_IMPORT_PREFIX}/lib/libtxnsp.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
