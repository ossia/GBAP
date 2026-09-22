#pragma once

#include <PathGenerator/PathGeneratorModel.hpp>

#include <halp/layout.hpp>

namespace spat
{

struct PathGenerator::ui
{
  using enum halp::colors;
  using enum halp::layouts;

  halp_meta(name, "Main")
  halp_meta(layout, hbox)
  halp_meta(background, background_dark)

  struct
  {
    halp_meta(layout, vbox)

    halp::item<&ins::speed> speed;
    halp::item<&ins::loop> loop;
    halp::item<&ins::path> path;
    halp::item<&ins::radius> radius;

    struct
    {
      halp_meta(layout, hbox)
      halp::item<&ins::ratio_x> ratio_x;
      halp::item<&ins::ratio_y> ratio_y;
      halp::item<&ins::phase> phase;
    } shape;

    struct
    {
      halp_meta(layout, hbox)
      halp::item<&ins::output_mode> output_mode;
      halp::item<&ins::z> z;
    } output;
  } controls;

  halp::item<&ins::pos> pos;
};

}
