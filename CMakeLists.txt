cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

if(NOT TARGET score_lib_base)
  include(ScoreExternalAddon)
endif()

if(NOT TARGET score_plugin_avnd)
  return()
endif()

project(score_addon_mbap LANGUAGES CXX)

###### MBAP ############

avnd_score_plugin_init(
  BASE_TARGET score_addon_mbap
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_mbap
  SOURCES
    MBAP/MBAP.hpp
    MBAP/MBAPModel.hpp
    MBAP/MBAPModel.cpp
    MBAP/GridWidget.hpp
    MBAP/GridWidget.cpp
    MBAP/MBAPUi.hpp
  TARGET MBAP
  MAIN_CLASS MBAP
  NAMESPACE Example
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_mbap
  SOURCES
    matrix/matrix.hpp
    matrix/matrixModel.hpp
    matrix/matrixModel.cpp
    matrix/matrixUi.hpp
  TARGET matrix
  MAIN_CLASS matrix
  NAMESPACE Example
)

avnd_score_plugin_finalize(
  BASE_TARGET score_addon_mbap
  PLUGIN_VERSION 1
  PLUGIN_UUID "1057641E-4253-4BD3-802B-09F063FF559C"
)

