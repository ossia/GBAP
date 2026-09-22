#include "PathGenerator.hpp"

#include <ossia/detail/math.hpp>

#include <cmath>

namespace spat {

constexpr float TWO_PI = ossia::two_pi;

static ossia::vec2f node_at(const std::vector<ossia::value>& nodes, std::size_t i) noexcept
{
  if(i < nodes.size())
    if(auto* v = nodes[i].target<ossia::vec2f>())
      return *v;
  return ossia::vec2f{0.5f, 0.5f};
}

void PathGenerator::operator()(const halp::tick_flicks& t) {
  const float raw = t.relative_position * inputs.speed;
  const float frac = raw - std::floor(raw);
  const bool reverse = inputs.loop && (static_cast<int64_t>(raw) % 2 != 0);

  // Every trajectory is a function of a single parameter that walks [0; 1]
  // forward, then backwards when ping-pong is on.
  const float u = reverse ? 1.f - frac : frac;

  auto& out = outputs.OutTab.value;
  const auto& sources = inputs.pos.value;

  const OutputMode mode = inputs.output_mode;
  const bool spatial = mode != OutputMode::XY;
  const float z = mode == OutputMode::XYZ ? inputs.z : 0.f;

  // Only touched when a source is added or removed, or when the mode changes
  // the type held in each value: the steady state reuses both the vector's
  // capacity and the vector already stored in each value.
  const bool holds_vec3 = !out.empty() && out.front().target<ossia::vec3f>();
  if(out.size() != sources.size() || (!out.empty() && holds_vec3 != spatial))
  {
    if(spatial)
      out.assign(sources.size(), ossia::value{ossia::vec3f{}});
    else
      out.assign(sources.size(), ossia::value{ossia::vec2f{}});
  }

  for(std::size_t i = 0; i < sources.size(); ++i)
  {
    const auto* nodes = sources[i].target<std::vector<ossia::value>>();
    if(!nodes || nodes->empty())
      continue;

    const ossia::vec2f p = path_point(*nodes, u);
    if(spatial)
      out[i].get<ossia::vec3f>() = ossia::vec3f{p[0], p[1], z};
    else
      out[i].get<ossia::vec2f>() = p;
  }
}

ossia::vec2f
PathGenerator::path_point(const std::vector<ossia::value>& nodes, float u) const noexcept
{
  const ossia::vec2f a = node_at(nodes, 0);
  // The second node is a handle: its distance from the first is the size of the
  // shape and its direction is where the shape starts, so every trajectory
  // passes through it. Radius is then an x/y aspect on top of that.
  const ossia::vec2f b = nodes.size() > 1 ? node_at(nodes, 1) : a;
  const float dx = b[0] - a[0];
  const float dy = b[1] - a[1];
  const float R = std::sqrt(dx * dx + dy * dy);
  const float rx = R * inputs.radius.value.x;
  const float ry = R * inputs.radius.value.y;
  const float phi = std::atan2(dy, dx) + TWO_PI * inputs.phase;

  switch(inputs.path)
  {
    case Linear:
    {
      return {a[0] + dx * u, a[1] + dy * u};
    }

    case Circle:
    {
      const float angle = TWO_PI * (1.f - u) + phi;
      return {a[0] + rx * std::cos(angle), a[1] + ry * std::sin(angle)};
    }

    case Spiral:
    {
      const float angle = 2.f * TWO_PI * u + phi;
      return {a[0] + rx * u * std::cos(angle), a[1] + ry * u * std::sin(angle)};
    }

    case Lissajous:
    {
      // Ratio X / Ratio Y are the two frequencies, Phase their offset.
      const float th = TWO_PI * u;
      return {
          a[0] + rx * std::sin(inputs.ratio_x * th + phi),
          a[1] + ry * std::sin(inputs.ratio_y * th)};
    }

    case Rose:
    {
      // r = cos(k.θ) with k = Ratio X, swept over Ratio Y turns so that
      // rational k/1 ratios close their petals.
      const float th = TWO_PI * inputs.ratio_y * u;
      const float r = std::cos(inputs.ratio_x * th);
      return {
          a[0] + rx * r * std::cos(th + phi), a[1] + ry * r * std::sin(th + phi)};
    }

    case Polygon:
    {
      // Perimeter of a regular Ratio X-gon inscribed in the radius ellipse.
      const int n = ossia::max(3, (int)inputs.ratio_x);
      const float s = u * n;
      const int k = ossia::min((int)s, n - 1);
      const float f = s - k;
      const float a0 = TWO_PI * k / n + phi;
      const float a1 = TWO_PI * (k + 1) / n + phi;
      const float x0 = std::cos(a0), y0 = std::sin(a0);
      const float x1 = std::cos(a1), y1 = std::sin(a1);
      return {a[0] + rx * (x0 + (x1 - x0) * f), a[1] + ry * (y0 + (y1 - y0) * f)};
    }
  }

  return a;
}

}
