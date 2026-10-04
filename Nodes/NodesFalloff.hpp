#pragma once

namespace spat
{
/**
 * Weight of a node in Voronoi mode, from how much farther it is than the
 * closest node. 1 for the closest node, 0 from twice the blur radius on.
 *
 * (1 - t^2)^2 reaches 0 at the edge of the blend zone with a flat slope, so a
 * node fades in from nothing as the point moves towards it.
 */
inline float voronoiWeight(float distFromClosest, float blurRadius) noexcept
{
  const float zone = 2.0f * blurRadius;
  if(!(distFromClosest < zone))
    return 0.0f;
  const float t = distFromClosest / zone;
  const float u = 1.0f - t * t;
  return u * u;
}
}
