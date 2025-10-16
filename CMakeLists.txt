cmake_minimum_required(VERSION 3.12)
project(OptiMA VERSION 1.0.0 LANGUAGES C CXX)
set(PROJECT_NAME "OptiMA")
set(BINARY_NAME "optima")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native")

option(ENABLE_MIP "Enable MIP solver support (requires the third-party library SCIP)" OFF)

set(ENABLE_MIP ON CACHE BOOL "Enable MIP solver support (forwarded from OptiMA)" FORCE)

add_subdirectory(TxnSP)

add_library(${BINARY_NAME} SHARED src/AgentModels/Agent.cpp src/AgentModels/AgentPool.cpp src/AgentModels/PostBox.cpp
src/Engine/AgentManager.cpp src/Engine/DefaultEstimator.cpp src/Engine/Driver.cpp src/Engine/Executor.cpp src/Engine/ExecutorState.cpp
src/Engine/Listener.cpp src/Engine/MultiAgentModel.cpp src/Engine/PluginManager.cpp src/Engine/Postmaster.cpp src/Engine/Scheduler.cpp
src/Engine/TransactionFactory.cpp src/Shared/Exceptions.cpp src/Shared/Message.cpp src/TransactionModels/TransactionQueue.cpp
src/TransactionModels/TransactionResult.cpp)

set_property(TARGET ${BINARY_NAME} PROPERTY CXX_STANDARD 20)

if(ENABLE_MIP)
  target_compile_definitions(${BINARY_NAME} PUBLIC ENABLE_MIP=1)
endif()

target_include_directories(${BINARY_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(${BINARY_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION 1.0.0
  PUBLIC_HEADER include/OptiMA/OptiMA.h    
)

target_link_libraries(${BINARY_NAME} PUBLIC txnsp)

include(GNUInstallDirs)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/OptiMAConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/OptiMAConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OptiMA
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/OptiMAConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OptiMA)

install(TARGETS ${BINARY_NAME}
  EXPORT OptiMATargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/OptiMA
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT OptiMATargets
  FILE OptiMATargets.cmake
  NAMESPACE OptiMA::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/OptiMA)
