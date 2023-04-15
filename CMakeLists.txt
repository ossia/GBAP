cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT TARGET score_lib_base)
  include(ScoreExternalAddon)
endif()

include(ScoreAvndHelper)

project(score_addon_mbap LANGUAGES CXX)

avnd_score_plugin_init(
  BASE_TARGET score_addon_mbap
)

avnd_score_plugin_add(
  BASE_TARGET score_addon_mbap
  SOURCES
    MBAP/MBAP.hpp
    MBAP/MBAPModel.hpp
    MBAP/MBAPModel.cpp
    MBAP/MBAPUi.hpp
  TARGET my_gain
  MAIN_CLASS MBAP
  NAMESPACE Example
)

avnd_score_plugin_finalize(
  BASE_TARGET score_addon_mbap
  PLUGIN_VERSION 1
  PLUGIN_UUID "5662EC9A-A0EA-4C5A-ADB3-7C86BF52B9A3"
)
