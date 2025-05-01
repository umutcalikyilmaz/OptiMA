cmake_minimum_required(VERSION 3.12)
project(MultiAgent VERSION 1.0.0 LANGUAGES C CXX)
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

add_subdirectory(TxnSP)

add_library(${BINARY_NAME} src/AgentModels/Agent.cpp src/AgentModels/AgentCore.cpp src/AgentModels/AgentTemplate.cpp
src/AgentModels/CorePool.cpp src/AgentModels/PostBox.cpp src/Engine/AgentManager.cpp src/Engine/DefaultEstimator.cpp
src/Engine/Driver.cpp src/Engine/Executor.cpp src/Engine/ExecutorState.cpp src/Engine/Listener.cpp src/Engine/MultiAgentModel.cpp
src/Engine/PluginManager.cpp src/Engine/Postmaster.cpp src/Engine/Scheduler.cpp src/Engine/TransactionFactory.cpp
src/PluginModels/PluginInstance.cpp src/Shared/Exceptions.cpp src/Shared/Memory.cpp
src/Shared/Message.cpp src/TransactionModels/Begin.cpp src/TransactionModels/Communicate.cpp src/TransactionModels/CreateAgent.cpp
src/TransactionModels/DestroyAgent.cpp src/TransactionModels/GetAgentInfo.cpp src/TransactionModels/ProceduralTransaction.cpp
src/TransactionModels/Process.cpp src/TransactionModels/ProcessingTransaction.cpp src/TransactionModels/StartAgent.cpp
src/TransactionModels/StopAgent.cpp src/TransactionModels/Transaction.cpp src/TransactionModels/TransactionQueue.cpp
src/TransactionModels/TransactionResult.cpp)

set_property(TARGET ${BINARY_NAME} PROPERTY CXX_STANDARD 20)
target_include_directories(${BINARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${BINARY_NAME} PRIVATE txnsp)

