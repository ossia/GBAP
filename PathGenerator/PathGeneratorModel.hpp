#pragma once

#include "Engine/Node/CommonWidgets.hpp"
#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/layout.hpp>
#include <halp/meta.hpp>

namespace Example {

class PathGenerator {
public:
  halp_meta(name, "PathGenerator")
  halp_meta(c_name, "pathgenerator")
  halp_meta(category, "Control/Spatialization")
  halp_meta(description, "This process generates points that move dynamically along a given trajectory.")
  halp_meta(manual_url, "https://ossia.io/score-docs/processes/pathgenerator.html")
  halp_meta(author, "Ahmed El Moudden, Société des Arts Technologiques")
  halp_meta(uuid, "95a53434-276a-42a3-bb6b-ee92462b9640")

  struct ins {
    halp::knob_f32<"Speed", halp::range{.min = 0., .max = 10., .init = 1.}> speed;
    halp::toggle<"Ping Pong"> loop;
    halp::enum_t<Control::Widgets::Path, "Path"> Path;

    struct {
      halp_meta(name, "Position")
      struct range {
        float min = 0;
        float max = 1;
        float init = 0.5;
      };
      enum widget { path_generator_xy };
      std::vector<ossia::value> value;
    } pos;

    halp::xy_spinboxes_f32<"Radius", halp::range{.min = 0., .max = 1, .init = 0.2}> radius;
  } inputs;

  struct {
    halp::val_port<"Output", std::vector<ossia::value>> OutTab;
  } outputs;

  using tick = halp::tick_flicks;

  void operator()(const halp::tick_flicks& t);
  void linear_path(const halp::tick_flicks& t, const float relativePos, const bool reverse);
  void circle_path(const halp::tick_flicks& t, const float relativePos, const bool reverse);
  void spiral_path(const halp::tick_flicks& t, const float relativePos, const bool reverse);
};

}
