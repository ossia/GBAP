#pragma once

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <vector>

namespace Example
{

class matrix
{
public:
  halp_meta(name, "matrix")
  halp_meta(category, "Audio/Spatialization")
  halp_meta(c_name, "matrix")
  halp_meta(uuid, "359C787C-E600-48E5-A6E6-AA83E94FCDF4")

  // Define inputs and outputs ports.
  // See the docs at https://github.com/celtera/avendish
  struct ins
  {
    halp::dynamic_audio_bus<"Input", double> audio;
    //halp::dynamic_audio_bus<"Input", double> weights;
    halp::val_port<"Weights", std::vector<float>> weights;
    halp::knob_f32<"Gain", halp::range{.min = 0., .max = 10., .init = 1.}> gain;
  } inputs;

  struct
  {
    halp::dynamic_audio_bus<"Output", double> audio;
  } outputs;

  using setup = halp::setup;
  void prepare(halp::setup info)
  {
    // Initialization, this method will be called with buffer size, etc.
  }

  // Do our processing for N samples
  using tick = halp::tick;

  // Defined in the .cpp
  void operator()(halp::tick t);

  // UI is defined in another file to keep things clear.
  ///struct ui;
};

}
