#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <halp/sample_accurate_controls.hpp>

#include <vector>

namespace Example
{

class GBAP
{
public:
  halp_meta(name, "GBAP")
  halp_meta(category, "Control/Spatialization")
  halp_meta(c_name, "GBAP")
  halp_meta(uuid, "81DB0C30-175B-422E-8C5F-87569D50548E")

  // Define inputs and outputs ports.
  // See the docs at https://github.com/celtera/avendish

  struct ins
  {
    halp::val_port<"Input Weights", std::vector<float>> weights;
    halp::knob_f32<"Gain", halp::range{.min = 0., .max = 10., .init = 0.5}> gain;
    struct : halp::knob_f32<"RollOff", halp::range{.min = -12., .max = 24., .init = 0}>
    {
      void update(GBAP& self) { self.rollOffV = exp(value / 8.6858); }
    } rolloff;
    halp::toggle<"Normalize"> normalize;
    struct : halp::spinbox_i32<"Sink X # ", halp::range{.min = 1, .max = 12., .init = 5}>
    {
      void update(GBAP& self)
      {
        self.intervX = (value > 1) ? (1 - self.inputs.sinkSize.value.x) / (value - 1)
                                         - self.inputs.sinkSize.value.x
                                   : 0;
        self.outputs.weights.value.resize(value * self.inputs.nSinksY.value);
        qDebug() << " interv X: " << self.intervX ;
      }
    } nSinksX;
    struct : halp::spinbox_i32<"Sink Y # ", halp::range{.min = 1, .max = 12., .init = 4}>
    {
      void update(GBAP& self)
      {
        self.intervY = (value > 1) ? (1 - self.inputs.sinkSize.value.y) / (value - 1)
                                         - self.inputs.sinkSize.value.y
                                   : 0;
        self.outputs.weights.value.resize(value * self.inputs.nSinksX.value);
        qDebug() << " interv Y: " << self.intervY ;
      }
    } nSinksY;
    halp::spinbox_i32<"Source # ", halp::range{.min = 1, .max = 12., .init = 2}> nSources;
    halp::spinbox_i32<"System Number", halp::range{0, 12, 0}> systemNumber;
    struct
        : halp::xy_spinboxes_f32<
              "Sink Size", halp::range{.min = 0, .max = 1., .init = 0.05}>
    {
      void update(GBAP& self)
      {
        self.intervX = (self.inputs.nSinksX.value > 1)
                           ? (1 - value.x) / (self.inputs.nSinksX.value - 1) - value.x
                           : 0;
        self.intervY = (self.inputs.nSinksY.value > 1)
                           ? (1 - value.y) / (self.inputs.nSinksY.value - 1) - value.y
                           : 0;
      }
    } sinkSize;
    halp::xy_spinboxes_f32<"Cursor Size", halp::range{.min = 0, .max = 1., .init = 0.04}>
        cursorSize;
    halp::xy_pad_f32<"Position", halp::range{.min = 0., .max = 1., .init = 0.5}> pos;

  } inputs;

  struct
  {
    halp::val_port<"Output Weights", std::vector<float>> weights;
  } outputs;

  using setup = halp::setup;
  void prepare(halp::setup info)
  {
    //outputs.weights.value.resize(2);
    // Initialization, this method will be called with buffer size, etc.
  }

  // Do our processing for N samples
  using tick = halp::tick;

  // Defined in the .cpp
  void operator()(halp::tick t);

  // UI is defined in another file to keep things clear.
  struct ui;

  float intervX{}, intervY{};
  std::vector<float> volumes;
  int nSinksprev{0};
  float rollOffV{1.};

  void rollOffArray(std::vector<float>& arr);
  void mult(std::vector<float>& arr, float scal);
};

}
