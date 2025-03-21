cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

if(NOT TARGET score_lib_base)
  include(ScoreExternalAddon)
endif()

if(NOT TARGET score_plugin_avnd)
  return()
endif()

project(score_addon_gbap LANGUAGES CXX)

###### GBAP ############

avnd_score_plugin_init(
  BASE_TARGET score_addon_gbap
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_gbap
  SOURCES
    GBAP/GBAP.hpp
    GBAP/GBAPModel.hpp
    GBAP/GBAPModel.cpp
    GBAP/GridWidget.hpp
    GBAP/GridWidget.cpp
    GBAP/GBAPUi.hpp
  TARGET GBAP
  MAIN_CLASS GBAP
  NAMESPACE Example
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_gbap
  SOURCES
    matrix/matrix.hpp
    matrix/matrixModel.hpp
    matrix/matrixModel.cpp
    matrix/matrixUi.hpp
  TARGET matrix
  MAIN_CLASS matrix
  NAMESPACE Example
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_gbap
  SOURCES
    MultiCursor/MultiCursor.hpp
    MultiCursor/MultiCursorModel.hpp
    MultiCursor/MultiCursorModel.cpp
  TARGET multicursormanager
  MAIN_CLASS MultiCursorManager
  NAMESPACE Example
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_gbap
  SOURCES
    PathGenerator/PathGenerator.hpp
    PathGenerator/PathGeneratorModel.hpp
    PathGenerator/PathGeneratorModel.cpp
    PathGenerator/PathGeneratorUi.hpp
  TARGET pathgenerator
  MAIN_CLASS PathGenerator
  NAMESPACE Example
)


avnd_score_plugin_finalize(
  BASE_TARGET score_addon_gbap
  PLUGIN_VERSION 1
  PLUGIN_UUID "1057641E-4253-4BD3-802B-09F063FF559C"
)

