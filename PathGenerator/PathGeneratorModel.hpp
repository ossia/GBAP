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

//! What the Output port carries: a vec2f, or a vec3f whose z is either zero or
//! the Z control. XY first, so that it stays the default of documents saved
//! before the port had a mode.
enum OutputMode
{
  XY,
  XY0,
  XYZ
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

    //! x/y aspect applied to the size the handle node sets: 1,1 is a circle.
    //! Keeps the name it had: reloadPortsInNewProcess matches ports by name, so
    //! renaming it would drop the value, the cables and the address of every
    //! document that already has one.
    halp::xy_spinboxes_f32<"Radius", halp::range{.min = 0., .max = 4, .init = 1.}>
        radius;

    // Shared by the curved paths, with a per-path meaning documented in path_point.
    halp::spinbox_i32<"Ratio X", halp::range{.min = 1, .max = 16, .init = 3}> ratio_x;
    halp::spinbox_i32<"Ratio Y", halp::range{.min = 1, .max = 16, .init = 2}> ratio_y;
    halp::knob_f32<"Phase", halp::range{.min = 0., .max = 1., .init = 0.}> phase;

    halp::enum_t<OutputMode, "Output mode"> output_mode;
    //! Third coordinate of the XYZ mode: the trajectory is planar, the height
    //! is set by hand.
    halp::hslider_f32<"Z", halp::range{.min = 0., .max = 1., .init = 0.}> z;
  } inputs;

  struct outs {
    halp::val_port<"Output", std::vector<ossia::value>> OutTab;
  } outputs;

  struct ui;

  using tick = halp::tick_flicks;

  void operator()(const halp::tick_flicks& t);

private:
  ossia::vec2f
  path_point(const std::vector<ossia::value>& nodes, float u) const noexcept;
};

}
