#include "PathGenerator.hpp"

namespace Example
{
void PathGenerator::operator()(const halp::tick_flicks& t)
{
  // Linear path
  if (inputs.Path == 0) {

    for (size_t v = 0; v < outputs.OutTab.value.size(); v++) {
      ossia::vec2f start = inputs.pos.value[v].get<std::vector<ossia::value>>()[0].get<ossia::vec2f>();
      ossia::vec2f end = inputs.pos.value[v].get<std::vector<ossia::value>>()[1].get<ossia::vec2f>();

      auto& out = outputs.OutTab.value[v].get<ossia::vec2f>();

      start[1] = 1 - start[1];
      end[1] = 1 - end[1];

      float relativePos = fmod(t.relative_position * inputs.speed, 1.0f);
      if (inputs.loop) {
        if (int(t.relative_position * inputs.speed) % 2 == 0) {
          out[0] = start[0] + (end[0] - start[0]) * relativePos;
          out[1] = start[1] + (end[1] - start[1]) * relativePos;
        } else {
          out[0] = end[0] - (end[0] - start[0]) * relativePos;
          out[1] = end[1] - (end[1] - start[1]) * relativePos;
        }
      } else {
          out[0] = start[0] + (end[0] - start[0]) * relativePos;
          out[1] = start[1] + (end[1] - start[1]) * relativePos;
      }
    }

    // Circle path
  } else if (inputs.Path == 1) {
    for (size_t v = 0; v < outputs.OutTab.value.size(); v++) {
      ossia::vec2f point = inputs.pos.value[v].get<std::vector<ossia::value>>()[0].get<ossia::vec2f>();

      auto& out = outputs.OutTab.value[v].get<ossia::vec2f>();

      point[1] = 1 - point[1];
      float relativePos = fmod(t.relative_position * inputs.speed, 1.0f);

      if (inputs.loop) {
        if (int(t.relative_position * inputs.speed) % 2 == 0) {
          out[0] = point[0] + inputs.radius.value.x * std::cos(t.relative_position * 2 * std::numbers::pi * inputs.speed);
          out[1] = point[1] + inputs.radius.value.y * std::sin(t.relative_position * 2 * std::numbers::pi * inputs.speed);
        } else {
          out[0] = point[0] + inputs.radius.value.x * std::cos((2 * std::numbers::pi - t.relative_position * 2 * std::numbers::pi) * inputs.speed);
          out[1] = point[1] + inputs.radius.value.y * std::sin((2 * std::numbers::pi - t.relative_position * 2 * std::numbers::pi) * inputs.speed);
        }
      } else {
          out[0] = point[0] + inputs.radius.value.x * std::cos(t.relative_position * 2 * std::numbers::pi * inputs.speed);
          out[1] = point[1] + inputs.radius.value.y * std::sin(t.relative_position * 2 * std::numbers::pi * inputs.speed);
      }
    }

    // Spiral path
  } else if (inputs.Path == 2) {
    float radX = inputs.radius.value.x * fmod(t.relative_position, 1.0f);
    float radY = inputs.radius.value.y * fmod(t.relative_position, 1.0f);

    for (size_t v = 0; v < outputs.OutTab.value.size(); v++) {
      ossia::vec2f point = inputs.pos.value[v].get<std::vector<ossia::value>>()[0].get<ossia::vec2f>();

      auto& out = outputs.OutTab.value[v].get<ossia::vec2f>();

      point[1] = 1 - point[1];
      float relativePos = fmod(t.relative_position * inputs.speed, 1.0f);

      if (inputs.loop) {
        if (int(t.relative_position * inputs.speed) % 2 == 0){
          out[0] = point[0] + radX * std::cos(relativePos * 4 * std::numbers::pi);
          out[1] = point[1] + radY * std::sin(relativePos * 4 * std::numbers::pi);
        } else {
          out[0] = point[0] + (inputs.radius.value.x-radX) * std::cos((1-relativePos ) * 4 * std::numbers::pi);
          out[1] = point[1] + (inputs.radius.value.y-radY) * std::sin((1-relativePos ) * 4 * std::numbers::pi);
        }
      } else {
          out[0] = point[0] + radX * std::cos(relativePos * 4 * std::numbers::pi);
          out[1] = point[1] + radY * std::sin(relativePos * 4 * std::numbers::pi);
      }
    }
  }
}
}
