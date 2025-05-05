#include "PathGenerator.hpp"
#include <cmath>
#include <numbers>

namespace spat {

// Constants for angle calculations
constexpr float TWO_PI = 2.0f * std::numbers::pi;
constexpr float FOUR_PI = 4.0f * std::numbers::pi;

void PathGenerator::operator()(const halp::tick_flicks& t) {
  float relativePos = std::fmod(t.relative_position * inputs.speed, 1.0f);
  bool reverse = static_cast<int>(t.relative_position * inputs.speed) % 2 != 0;

  outputs.OutTab.value.clear();

  // Initialize output positions with first point from each input
  for (const auto& val : inputs.pos.value) {
    const auto& vec = val.get<std::vector<ossia::value>>();
    outputs.OutTab.value.push_back(vec[0].get<ossia::vec2f>());
  }

  // Dispatch to path function
  switch (inputs.path) {
    case 0: linear_path(t, relativePos, reverse); break;
    case 1: circle_path(t, relativePos, reverse); break;
    case 2: spiral_path(t, relativePos, reverse); break;
    default: break;
  }
}

// Linear interpolation between two points
void PathGenerator::linear_path(const halp::tick_flicks&, float relativePos, bool reverse) {
  for (size_t i = 0; i < outputs.OutTab.value.size(); ++i) {
    const auto& vec = inputs.pos.value[i].get<std::vector<ossia::value>>();
    const auto& start = vec[0].get<ossia::vec2f>();
    const auto& end   = vec[1].get<ossia::vec2f>();
    auto& out = outputs.OutTab.value[i].get<ossia::vec2f>();

    if (inputs.loop && reverse) {
      out = {
          end[0] - (end[0] - start[0]) * relativePos,
          end[1] - (end[1] - start[1]) * relativePos
      };
    } else {
      out = {
          start[0] + (end[0] - start[0]) * relativePos,
          start[1] + (end[1] - start[1]) * relativePos
      };
    }
  }
}

// Generate circular motion around a point
void PathGenerator::circle_path(const halp::tick_flicks&, float relativePos, bool reverse) {
  float angle = TWO_PI * (inputs.loop && reverse ? relativePos : 1.0f - relativePos);

  for (size_t i = 0; i < outputs.OutTab.value.size(); ++i) {
    const auto& center = inputs.pos.value[i].get<std::vector<ossia::value>>()[0].get<ossia::vec2f>();
    auto& out = outputs.OutTab.value[i].get<ossia::vec2f>();

    out = {
        center[0] + inputs.radius.value.x * std::cos(angle),
        center[1] + inputs.radius.value.y * std::sin(angle)
    };
  }
}

// Generate expanding spiral motion around a point
void PathGenerator::spiral_path(const halp::tick_flicks&, float relativePos, bool reverse) {
  float angle = FOUR_PI * (inputs.loop && reverse ? 1.0f - relativePos : relativePos);
  float radX = inputs.radius.value.x * relativePos;
  float radY = inputs.radius.value.y * relativePos;

  for (size_t i = 0; i < outputs.OutTab.value.size(); ++i) {
    const auto& center = inputs.pos.value[i].get<std::vector<ossia::value>>()[0].get<ossia::vec2f>();
    auto& out = outputs.OutTab.value[i].get<ossia::vec2f>();

    if (inputs.loop && reverse) {
      out = {
          center[0] + (inputs.radius.value.x - radX) * std::cos(angle),
          center[1] + (inputs.radius.value.y - radY) * std::sin(angle)
      };
    } else {
      out = {
          center[0] + radX * std::cos(angle),
          center[1] + radY * std::sin(angle)
      };
    }
  }
}

}
