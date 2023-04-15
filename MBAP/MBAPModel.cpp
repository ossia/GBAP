#include "MBAP.hpp"

namespace Example
{
void MBAP::operator()(halp::tick t)
{
  // Process the input buffer
  for(int i = 0; i < inputs.audio.channels; i++)
  {
    auto* in = inputs.audio[i];
    auto* out = outputs.audio[i];

    for(int j = 0; j < t.frames; j++)
    {
      out[j] = inputs.gain * in[j];
    }
  }
}
}
