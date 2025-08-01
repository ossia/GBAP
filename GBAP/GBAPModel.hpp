#pragma once

#include <ossia/network/value/value.hpp>

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <halp/sample_accurate_controls.hpp>

#include <vector>

namespace spat
{

class GBAP
{
public:
  halp_meta(name, "GBAP")
  halp_meta(category, "Control/Spatialization")
  halp_meta(c_name, "GBAP")
  halp_meta(uuid, "81DB0C30-175B-422E-8C5F-87569D50548E")
  halp_meta(
      author,
      "Pía Baltazar, Ahmed El Moudden, Sarah Al Mamoun, Jean-Michaël Celerier, Société "
      "des Arts Technologiques");
  halp_meta(
      description, "Grid-based value ponderation, useful for sound spatialization.");
  halp_meta(manual_url, "https://ossia.io/score-docs/processes/gbap.html")

  struct ins
  {
    halp::val_port<"Input Weights", std::vector<float>> weights;
    halp::val_port<"Input Multicursor", std::vector<ossia::vec2f>> MultiTab;
    halp::knob_f32<"Gain", halp::range{.min = 0., .max = 1., .init = 1.}> gain;

    struct : halp::knob_f32<"RollOff", halp::range{.min = 0., .max = 24., .init = 6}>
    {
      void update(GBAP& self) { self.rollOffV = exp(value / 8.6858); }
    } rolloff;

    halp::toggle<"Normalize"> normalize;

    struct : halp::spinbox_i32<"Sink X # ", halp::range{.min = 1, .max = 12., .init = 5}>
    {
      void update(GBAP& self)
      {
        self.updateInterv();

        self.outputs.weights.value.resize(value * self.inputs.nSinksY.value);
      }
    } nSinksX;

    struct : halp::spinbox_i32<"Sink Y # ", halp::range{.min = 1, .max = 12., .init = 4}>
    {
      void update(GBAP& self)
      {
        self.updateInterv();

        self.outputs.weights.value.resize(value * self.inputs.nSinksX.value);
      }
    } nSinksY;

    halp::spinbox_i32<"Source # ", halp::range{.min = 1, .max = 12., .init = 2}> nSources;
    halp::spinbox_i32<"System Number", halp::range{0, 12, 0}> systemNumber;

    struct
        : halp::xy_spinboxes_f32<
              "Sink Size", halp::range{.min = 0., .max = 1., .init = 0.05}>
    {
      void update(GBAP& self)
      {
        self.updateInterv();
      }
    } sinkSize;

    halp::xy_spinboxes_f32<"Cursor Size", halp::range{.min = 0., .max = 1., .init = 0.04}> cursorSize;
    halp::xy_spinboxes_f32<"Position", halp::range{.min = 0., .max = 1., .init = 0.5}> pos;

  } inputs;

  struct
  {
    halp::val_port<"Output Weights", std::vector<ossia::value>> weights;
  } outputs;

  using setup = halp::setup;
  void prepare(halp::setup info)
  {}

  using tick = halp::tick;
  void operator()(halp::tick t);

  struct ui;

  float intervX{}, intervY{};

  std::vector<ossia::value> volumes;

  int nSinksprev{0};
  float rollOffV{1.};

  float sinkX{}, sinkY{};
  void rollOffArray(std::vector<ossia::value>& arr);
  void mult(std::vector<ossia::value>& arr, float scal);
  void normalizeArray(std::vector<ossia::value>& arr);
  void updateInterv();
};

}
