#include "PathGenerator.hpp"
#include <cmath>
#include <numbers>

namespace Example {

constexpr float TWO_PI = 2.0f * std::numbers::pi;
constexpr float FOUR_PI = 4.0f * std::numbers::pi;

void PathGenerator::operator()(const halp::tick_flicks& t) {

  float relativePos = fmod(t.relative_position * inputs.speed, 1.0f);
  bool reverse = (int(t.relative_position * inputs.speed) % 2) != 0;

  switch (inputs.Path) {
    case 0: LinearPath(t, relativePos, reverse); break;
    case 1: CirclePath(t, relativePos, reverse); break;
    case 2: SpiralPath(t, relativePos, reverse); break;
    default: break;
  }
}

void PathGenerator::LinearPath(const halp::tick_flicks& t, float relativePos, bool reverse) {

  for (size_t v = 0; v < outputs.OutTab.value.size(); v++) {
    auto start = inputs.pos.value[v].get<std::vector<ossia::value>>()[0].get<ossia::vec2f>();
    auto end   = inputs.pos.value[v].get<std::vector<ossia::value>>()[1].get<ossia::vec2f>();
    auto& out  = outputs.OutTab.value[v].get<ossia::vec2f>();

    if (inputs.loop && reverse) {
      out[0] = end[0] - (end[0] - start[0]) * relativePos;
      out[1] = end[1] - (end[1] - start[1]) * relativePos;
    } else {
      out[0] = start[0] + (end[0] - start[0]) * relativePos;
      out[1] = start[1] + (end[1] - start[1]) * relativePos;
    }
  }
}

void PathGenerator::CirclePath(const halp::tick_flicks& t, float relativePos, bool reverse) {

  float angle = inputs.loop && reverse ? (TWO_PI - relativePos * TWO_PI) : (relativePos * TWO_PI);

  for (size_t v = 0; v < outputs.OutTab.value.size(); v++) {
    auto point = inputs.pos.value[v].get<std::vector<ossia::value>>()[0].get<ossia::vec2f>();
    auto& out  = outputs.OutTab.value[v].get<ossia::vec2f>();

    out[0] = point[0] + inputs.radius.value.x * std::cos(angle);
    out[1] = point[1] + inputs.radius.value.y * std::sin(angle);
  }
}

void PathGenerator::SpiralPath(const halp::tick_flicks& t, float relativePos, bool reverse) {

  float angle = relativePos * FOUR_PI;
  float radX = inputs.radius.value.x * relativePos;
  float radY = inputs.radius.value.y * relativePos;

  for (size_t v = 0; v < outputs.OutTab.value.size(); v++) {
    auto point = inputs.pos.value[v].get<std::vector<ossia::value>>()[0].get<ossia::vec2f>();
    auto& out  = outputs.OutTab.value[v].get<ossia::vec2f>();

    if (inputs.loop && reverse) {
      out[0] = point[0] + (inputs.radius.value.x - radX) * std::cos((1 - relativePos) * FOUR_PI);
      out[1] = point[1] + (inputs.radius.value.y - radY) * std::sin((1 - relativePos) * FOUR_PI);
    } else {
      out[0] = point[0] + radX * std::cos(angle);
      out[1] = point[1] + radY * std::sin(angle);
    }
  }
}

}
