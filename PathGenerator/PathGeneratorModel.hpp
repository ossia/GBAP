#pragma once

#include <ossia/network/value/value.hpp>

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/layout.hpp>
#include <halp/meta.hpp>

namespace spat {

//! Kept in sync with score::QGraphicsPathGeneratorXY::Path, which draws the
//! same curves in the editor.
enum Path
{
  Linear,
  Circle,
  Spiral,
  Lissajous,
  Rose,
  Polygon
};
class PathGenerator {
public:
  halp_meta(name, "PathGenerator")
  halp_meta(c_name, "pathgenerator")
  halp_meta(category, "Spatialization")
  halp_meta(description, "This process generates points that move dynamically along a given trajectory.")
  halp_meta(manual_url, "https://ossia.io/score-docs/processes/pathgenerator.html")
  halp_meta(author, "Ahmed El Moudden, Société des Arts Technologiques")
  halp_meta(uuid, "95a53434-276a-42a3-bb6b-ee92462b9640")

  struct ins {
    halp::knob_f32<"Speed", halp::range{.min = 0., .max = 10., .init = 1.}> speed;
    halp::toggle<"Ping Pong"> loop;
    halp::enum_t<Path, "Path"> path;

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

    // Shared by the curved paths, with a per-path meaning documented in path_point.
    halp::spinbox_i32<"Ratio X", halp::range{.min = 1, .max = 16, .init = 3}> ratio_x;
    halp::spinbox_i32<"Ratio Y", halp::range{.min = 1, .max = 16, .init = 2}> ratio_y;
    halp::knob_f32<"Phase", halp::range{.min = 0., .max = 1., .init = 0.}> phase;
  } inputs;

  struct {
    halp::val_port<"Output", std::vector<ossia::value>> OutTab;

    //! Position along the trajectory, in [0; 1] with the ping-pong already
    //! applied: this is what the editor needs to draw the moving point.
    halp::hbargraph_f32<"Progress", halp::range{.min = 0., .max = 1., .init = 0.}> progress;
  } outputs;

  using tick = halp::tick_flicks;

  void operator()(const halp::tick_flicks& t);

private:
  ossia::vec2f
  path_point(const std::vector<ossia::value>& nodes, float u) const noexcept;
};

}
