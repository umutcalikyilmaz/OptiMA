cmake_minimum_required(VERSION 3.5.0)

SET(PROJECT_NAME "TxnSP")
SET(BINARY_NAME "txnsp")

project(${PROJECT_NAME} VERSION 1.0.5 LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native")

add_library(${BINARY_NAME} src/PoolingModels/SchedulePool.cpp src/PoolingModels/SubsetPool.cpp
src/ProblemModels/Problem.cpp src/ProblemModels/Schedule.cpp src/ProblemModels/Subset.cpp src/Shared/SharedModels.cpp
src/Solvers/DPSolver.cpp src/Solvers/ESSolver.cpp src/Solvers/MIPSolver.cpp src/Solvers/SASolver.cpp
src/Structs/SolverOutput.cpp src/Testers/Analyzer.cpp src/Testers/Evaluator.cpp)

set_property(TARGET ${BINARY_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${BINARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${BINARY_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/scip/lib/libscip.so)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/scip/include)

