cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(score_addon_gbap LANGUAGES CXX)

# Max/MSP externals link the static CRT (/MT) on Windows, as the official
# max-sdk-base requires. Set it for the whole addon -- before Avendish and the
# object library are created -- so every target shares one runtime; mixing /MT
# and /MD trips MSVC with LNK2038. (CMP0091 is NEW via cmake_minimum_required.)
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Use the Avendish the host (ossia score) provides; otherwise fetch it.
find_package(Avendish QUIET)
if(NOT Avendish_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    Avendish
    GIT_REPOSITORY "https://github.com/celtera/avendish"
    GIT_TAG 7eafe173)
  FetchContent_Populate(Avendish)
  list(APPEND CMAKE_PREFIX_PATH "${avendish_SOURCE_DIR}")
  find_package(Avendish REQUIRED)
endif()

avnd_addon_init(NAME score_addon_gbap)

avnd_addon_object(
  BASE score_addon_gbap C_NAME GBAP CLASS GBAP NAMESPACE spat
  SOURCES GBAP/GBAP.hpp GBAP/GBAPModel.hpp GBAP/GBAPModel.cpp GBAP/GridWidget.hpp GBAP/GBAPUi.hpp)

avnd_addon_object(
  BASE score_addon_gbap C_NAME matrix CLASS Matrix NAMESPACE spat
  SOURCES matrix/matrix.hpp matrix/matrixModel.hpp matrix/matrixModel.cpp matrix/matrixUi.hpp)

avnd_addon_object(
  BASE score_addon_gbap C_NAME multicursormanager CLASS MultiCursorManager NAMESPACE spat
  SOURCES MultiCursor/MultiCursor.hpp MultiCursor/MultiCursorModel.hpp MultiCursor/MultiCursorModel.cpp)

avnd_addon_object(
  BASE score_addon_gbap C_NAME pathgenerator CLASS PathGenerator NAMESPACE spat
  SOURCES PathGenerator/PathGenerator.hpp PathGenerator/PathGeneratorModel.hpp PathGenerator/PathGeneratorModel.cpp)

avnd_addon_object(
  BASE score_addon_gbap C_NAME Nodes CLASS Nodes NAMESPACE spat
  SOURCES Nodes/Nodes.hpp Nodes/NodesModel.hpp Nodes/NodesUi.hpp Nodes/NodesWidget.hpp)

avnd_addon_finalize(NAME score_addon_gbap UUID 1057641E-4253-4BD3-802B-09F063FF559C VERSION 1)

if(COMMAND score_optimize_in_debug_mode)
  score_optimize_in_debug_mode(${PROJECT_NAME})
endif()
