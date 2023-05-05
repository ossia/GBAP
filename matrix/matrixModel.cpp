#include "matrix.hpp"

namespace Example
{

void matrix::operator()(halp::tick t)
{
  // Process the input buffer
  for(int i = 0; i < inputs.audio.channels; i++)
  {
    auto* in = inputs.audio[i];
    for(int k = 0; k < outputs.audio.channels; k++)
    {
      //auto* weight = (inputs.weights.channels > k) ?
      //      inputs.weights[k] : inputs.weights[inputs.weights.channels-1];
      auto weight = (inputs.weights.value.size() > k) ? inputs.weights.value[k]
                          : inputs.weights.value[inputs.weights.value.size()-1];

      auto* out = outputs.audio[k];

      for(int j = 0; j < t.frames; j++)
      {
        //out[j] = inputs.gain * weight[j] * in[j];
        out[j] = inputs.gain * weight * in[j];
      }
    }
  }
}
}
