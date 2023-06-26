#include "matrix.hpp"

namespace Example
{

void matrix::operator()(halp::tick t)
{
  // Update the output audio channel count if this changed
  int out_channels = inputs.outs.value + inputs.offs.value ;
  if(out_channels != this->prev_outs)
  {
    outputs.audio.request_channels(out_channels);
    prev_outs = out_channels;
    return;
  }

  // Apply the matrix to the audio input
  for(int i = 0; i < inputs.audio.channels; i++)
  {
    auto* in = inputs.audio[i];
    for(int k = 0; k < outputs.audio.channels; k++)
    {
      auto weight = (k < inputs.offs.value ) ? 0.f :
                    (inputs.weights.value.size() > k)
                        ? inputs.weights.value[k]
                        : inputs.weights.value[inputs.weights.value.size() - 1];

      auto* out = outputs.audio[k];

      for(int j = 0; j < t.frames; j++)
      {
        out[j] = inputs.gain * weight * in[j];
      }
    }
  }
}
}
