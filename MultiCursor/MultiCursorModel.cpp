#include "MultiCursor.hpp"

namespace spat
{
void MultiCursorManager::operator()(halp::tick t)
{
  outputs.out.value = inputs.pos.value;
}
}
