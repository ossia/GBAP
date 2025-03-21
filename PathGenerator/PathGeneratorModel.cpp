#include "PathGenerator.hpp"
#include <iostream>
#include <cmath>

namespace Example
{
void PathGenerator::operator()(const halp::tick_flicks& t)
{
  // Linear path
  if (inputs.Path == 0) {
    std::cout << (int)t.relative_position << std::endl;

    for (size_t v = 0; v < outputs.OutTab.value.size(); v++) {
      ossia::vec2f start = inputs.pos.value[v].get<std::vector<ossia::value>>()[0].get<ossia::vec2f>();
      ossia::vec2f end = inputs.pos.value[v].get<std::vector<ossia::value>>()[1].get<ossia::vec2f>();

      start[1] = 1 - start[1];
      end[1] = 1 - end[1];

      float relativePos = fmod(t.relative_position * inputs.speed, 1.0f);
      if (inputs.loop) {
        if ((int)fmod(t.relative_position * inputs.speed, 2.0f) == 0) {
          outputs.OutTab.value[v].get<ossia::vec2f>()[0] = start[0] + (end[0] - start[0]) * relativePos;
          outputs.OutTab.value[v].get<ossia::vec2f>()[1] = start[1] + (end[1] - start[1]) * relativePos;
        } else {
          outputs.OutTab.value[v].get<ossia::vec2f>()[0] = end[0] - (end[0] - start[0]) * relativePos;
          outputs.OutTab.value[v].get<ossia::vec2f>()[1] = end[1] - (end[1] - start[1]) * relativePos;
        }
      } else {
        outputs.OutTab.value[v].get<ossia::vec2f>()[0] = start[0] + (end[0] - start[0]) * relativePos;
        outputs.OutTab.value[v].get<ossia::vec2f>()[1] = start[1] + (end[1] - start[1]) * relativePos;
      }
    }

    // Circle path
  } else if (inputs.Path == 1) {
    for (size_t v = 0; v < outputs.OutTab.value.size(); v++) {
      ossia::vec2f point = inputs.pos.value[v].get<std::vector<ossia::value>>()[0].get<ossia::vec2f>();

      point[1] = 1 - point[1];
      float relativePos = fmod(t.relative_position * inputs.speed, 1.0f);

      if (inputs.loop) {
        if ((int)fmod(t.relative_position * inputs.speed, 2.0f) == 0) {
          outputs.OutTab.value[v].get<ossia::vec2f>()[0] = point[0] + inputs.radius.value.x * std::cos(t.relative_position * 2 * M_PI * inputs.speed);
          outputs.OutTab.value[v].get<ossia::vec2f>()[1] = point[1] + inputs.radius.value.y * std::sin(t.relative_position * 2 * M_PI * inputs.speed);
        } else {
          outputs.OutTab.value[v].get<ossia::vec2f>()[0] = point[0] + inputs.radius.value.x * std::cos((2 * M_PI - t.relative_position * 2 * M_PI) * inputs.speed);
          outputs.OutTab.value[v].get<ossia::vec2f>()[1] = point[1] + inputs.radius.value.y * std::sin((2 * M_PI - t.relative_position * 2 * M_PI) * inputs.speed);
        }
      } else {
        outputs.OutTab.value[v].get<ossia::vec2f>()[0] = point[0] + inputs.radius.value.x * std::cos(t.relative_position * 2 * M_PI * inputs.speed);
        outputs.OutTab.value[v].get<ossia::vec2f>()[1] = point[1] + inputs.radius.value.y * std::sin(t.relative_position * 2 * M_PI * inputs.speed);
      }
    }

    // Spiral path
  } else if (inputs.Path == 2) {
    float radX = inputs.radius.value.x * fmod(t.relative_position, 1.0f);
    float radY = inputs.radius.value.y * fmod(t.relative_position, 1.0f);

    for (size_t v = 0; v < outputs.OutTab.value.size(); v++) {
      ossia::vec2f point = inputs.pos.value[v].get<std::vector<ossia::value>>()[0].get<ossia::vec2f>();

      point[1] = 1 - point[1];
      float relativePos = fmod(t.relative_position * inputs.speed, 1.0f);

      if (inputs.loop) {
        if ((int)fmod(t.relative_position, 2.0f) == 0) {
          outputs.OutTab.value[v].get<ossia::vec2f>()[0] = point[0] + radX * std::cos(relativePos * 4 * M_PI);
          outputs.OutTab.value[v].get<ossia::vec2f>()[1] = point[1] + radY * std::sin(relativePos * 4 * M_PI);
        } else {
          std::cout<<"HE"<<std::endl;
          outputs.OutTab.value[v].get<ossia::vec2f>()[0] = point[0] + (inputs.radius.value.x-radX) * std::cos((1-relativePos ) * 4 * M_PI);
          outputs.OutTab.value[v].get<ossia::vec2f>()[1] = point[1] + (inputs.radius.value.y-radY) * std::sin((1-relativePos ) * 4 * M_PI);
        }
      } else {
        outputs.OutTab.value[v].get<ossia::vec2f>()[0] = point[0] + radX * std::cos(relativePos * 4 * M_PI);
        outputs.OutTab.value[v].get<ossia::vec2f>()[1] = point[1] + radY * std::sin(relativePos * 4 * M_PI);
      }
    }
  }
}
}
