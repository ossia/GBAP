#pragma once

#include <QPainter>

#include <avnd/wrappers/controls.hpp>
#include <halp/custom_widgets.hpp>
#include <halp/value_types.hpp>

#include <cmath>

#include <limits>
#include <vector>

namespace spat
{

struct NodesWidget
{
  static constexpr double width() { return 400.; }
  static constexpr double height() { return 400.; }

  void paint(auto ctx)
  {
    // Draw background
    ctx.set_font("Monospace");
    ctx.set_font_size(8);
    ctx.set_fill_color({20, 20, 20, 255});
    ctx.begin_path();
    ctx.draw_rect(0., 0., width(), height());
    ctx.fill();

    // Draw Voronoi cells if in Voronoi mode
    if(voronoiMode)
    {
      draw_voronoi_cells(ctx);
    }

    // Draw nodes
    if(executing)
    {
      ctx.set_stroke_color({255, 200, 100, 255});
      ctx.set_stroke_width(3.);
      ctx.set_fill_color({100, 80, 50, 100});
      for(const auto& node : runtimeNodes)
      {
        const double centerX = node.x * width();
        const double centerY = node.y * height();
        const double radius = voronoiMode ? 5.0 : node.z * std::min(width(), height());

        // Draw weight as transparent overlay
        ctx.begin_path();
        ctx.draw_circle(centerX, centerY, radius);
        ctx.fill();
        ctx.stroke();
      }
    }

    int i = 0;
    for(const auto& [x01, y01, r01] : nodes)
    {
      const double centerX = x01 * width();
      const double centerY = y01 * height();
      const double radius = voronoiMode ? 5.0 : r01 * std::min(width(), height());

      // Draw node circle
      if (i == selectedNode)
      {
        if(executing)
        {
          ctx.unset_stroke();
        }
        else
        {
          ctx.set_stroke_color({255, 200, 100, 255});
          ctx.set_stroke_width(3.);
        }
        ctx.set_fill_color({100, 80, 50, 100});
      }
      else
      {
        if(executing)
        {
          ctx.unset_stroke();
        }
        else
        {
          ctx.set_stroke_color({150, 150, 150, 255});
          ctx.set_stroke_width(1.);
        }
        ctx.set_fill_color({60, 60, 60, 100});
      }

      ctx.begin_path();
      ctx.draw_circle(centerX, centerY, radius);
      ctx.fill();
      ctx.stroke();

      // Draw node number
      ctx.set_fill_color({255, 255, 255, 255});
      ctx.begin_path();
      ctx.draw_text(
          centerX - radius - 5., centerY - radius, 2 * radius, 2 * radius,
          std::to_string(i));
      ctx.fill();

      i++;
    }

    // Draw input point (from value)
    ctx.set_fill_color({100, 200, 255, 255});
    ctx.begin_path();
    ctx.draw_circle(cursor.x * width(), cursor.y * height(), 5);
    ctx.fill();

    // Draw runtime input point if different
    if(executing
       && (std::abs(runtime_cursor.x - cursor.x) > 0.01f
           || std::abs(runtime_cursor.y - cursor.y) > 0.01f))
    {
      ctx.set_stroke_color({255, 100, 100, 255});
      ctx.set_stroke_width(2.);
      ctx.set_fill_color({255, 100, 100, 128});
      ctx.begin_path();
      ctx.draw_circle(runtime_cursor.x * width(), runtime_cursor.y * height(), 7);
      ctx.fill();
      ctx.stroke();
    }
  }

  // REFACTORME
  std::vector<float> compute_weights() const
  {
    std::vector<float> weights;
    weights.reserve(nodes.size());

    float totalWeight = 0.0f;

    for(const auto& [nx, ny, nr] : nodes)
    {
      float dx = cursor.x - nx;
      float dy = cursor.y - ny;
      float dist = std::sqrt(dx * dx + dy * dy);

      float weight = 0.0f;
      if(dist < nr)
      {
        weight = 1.0f - (dist / nr);
        weight = std::max(0.0f, std::min(1.0f, weight));
      }

      weights.push_back(weight);
      totalWeight += weight;
    }

    // Normalize if total > 1
    if(totalWeight > 1.0f)
    {
      for(auto& w : weights)
      {
        w /= totalWeight;
      }
    }

    return weights;
  }

  float distance(double x1, double y1, double x2, double y2) const
  {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
  }

  int findNodeAt(double x, double y) const
  {
    for (int i = 0; i < std::ssize(nodes); ++i)
    {
      double nodeX = nodes[i].x * width();
      double nodeY = nodes[i].y * height();

      if(distance(x, y, nodeX, nodeY) < 15.0) // 15 pixel threshold
      {
        return i;
      }
    }
    return -1;
  }

  bool mouse_press(double x, double y)
  {
    // Check if clicking on input point
    double inputX = cursor.x * width();
    double inputY = cursor.y * height();
    if (distance(x, y, inputX, inputY) < 10.0)
    {
      dragging = true;
      mouse_move(x, y);
      return true;
    }

    // Check if clicking on a node
    int nodeIndex = findNodeAt(x, y);
    if (nodeIndex >= 0)
    {
      transaction.start();
      selectedNode = nodeIndex;
      return true;
    }

    // Create new node
    nodes.push_back(
        {static_cast<float>(x / width()), static_cast<float>(y / height()), 0.1f});
    selectedNode = std::ssize(nodes) - 1;
    return true;
  }

  void mouse_move(double x, double y)
  {
    if (dragging)
    {
      cursor
          = {static_cast<float>(std::clamp(x / width(), 0.0, 1.0)),
             static_cast<float>(std::clamp(y / height(), 0.0, 1.0))};
      if(on_input_changed)
        on_input_changed();
    }
    else if (selectedNode >= 0 && selectedNode < std::ssize(nodes))
    {
      nodes[selectedNode].x = static_cast<float>(std::clamp(x / width(), 0.0, 1.0));
      nodes[selectedNode].y = static_cast<float>(std::clamp(y / height(), 0.0, 1.0));

      transaction.update(nodes);
    }
  }

  void mouse_release(double x, double y)
  {
    mouse_move(x, y);
    if(selectedNode >= 0)
    {
      transaction.commit();
    }
    dragging = false;
    selectedNode = -1;
  }

  bool key_press(int key)
  {
    // FIXME
    if (key == 16777223 && selectedNode >= 0) // Qt::Key_Delete
    {
      nodes.erase(nodes.begin() + selectedNode);
      selectedNode = -1;

      transaction.start();
      transaction.update(nodes);
      transaction.commit();
      return true;
    }
    return false;
  }

  void wheel(double x, double y, double delta)
  {
    // Only allow radius adjustment in distance mode
    if (!voronoiMode)
    {
      int nodeIndex = findNodeAt(x, y);
      if (nodeIndex >= 0)
      {
        // Adjust radius with wheel
        nodes[nodeIndex].z += delta * 0.0001f;
        nodes[nodeIndex].z = std::clamp(nodes[nodeIndex].z, 0.01f, 0.5f);

        transaction.start();
        transaction.update(nodes);
        transaction.commit();
      }
    }
  }

  void draw_voronoi_cells(auto ctx)
  {
    auto& nodes = executing ? runtimeNodes : this->nodes;
    if(nodes.empty())
      return;
    // Visualization of Voronoi cells with smooth transitions using pixmap
    const int w = width();
    const int h = height();
    const float blurRadius = globalRadius;
    static constexpr int step = 8;

    // Create RGBA pixmap
    pixmap.clear();
    pixmap.resize(w * h * 4 / step);
    distances.resize(nodes.size());
    weights.resize(nodes.size());

    // Render voronoi cells to pixmap
    int ox = 0, oy = 0;
    ;
    for(int py = 0; py < h; py += step)
    {
      oy = py / step;
      for(int px = 0; px < w; px += step)
      {
        ox = px / step;
        float x = px / (float)w;
        float y = py / (float)h;

        // Calculate distances to all nodes
#pragma omp simd
        for (int i = 0; i < nodes.size(); ++i)
        {
          distances[i] = std::hypot(x - nodes[i].x, y - nodes[i].y);
        }

        // Find the closest node distance
        float minDist = std::numeric_limits<float>::max();
#pragma omp simd
        for (float dist : distances)
        {
          minDist = std::min(minDist, dist);
        }
        
        // Calculate weights using same algorithm as processing
        float totalWeight = 0.0f;
        for (int i = 0; i < nodes.size(); ++i)
        {
          float weight = 0.0f;
          float distFromClosest = distances[i] - minDist;
          
          if (distFromClosest < 2.0f * blurRadius)
          {
            // Node is within transition zone
            float t = distFromClosest / (2.0f * blurRadius);
            weight = std::exp(-3.0f * t * t); // Gaussian-like falloff
          }

          weights[i] = weight;
          totalWeight += weight;
        }
        
        // Normalize weights
        if(totalWeight > 0.0f)
        {
#pragma omp simd
          for (auto& w : weights)
          {
            w /= totalWeight;
          }
        }
        
        // Blend colors based on weights
        float r = 0, g = 0, b = 0;
#pragma omp simd
        for (int i = 0; i < nodes.size(); ++i)
        {
          if (weights[i] > 0.0f)
          {
            r += weights[i] * ((i * 67) % 128 + 40);
            g += weights[i] * ((i * 149) % 128 + 40);
            b += weights[i] * ((i * 211) % 128 + 40);
          }
        }
        
        // Write to pixmap (RGBA format)
        int idx = (oy * w / step + ox) * 4;
        pixmap[idx] = (unsigned char)r;
        pixmap[idx + 1] = (unsigned char)g;
        pixmap[idx + 2] = (unsigned char)b;
        pixmap[idx + 3] = 255; // Alpha
      }
    }

    // Draw the pixmap
    ctx.draw_bytes(0, 0, w, h, pixmap.data(), w / step, h / step, true);
  }

  void start() { }

  void reset()
  {
    executing = false;
    runtime_cursor = {};
    runtimeNodes.clear();
  }

  std::vector<halp::xyz_type<float>> nodes;
  std::vector<halp::xyz_type<float>> runtimeNodes;
  halp::transaction<std::vector<halp::xyz_type<float>>> transaction;
  std::vector<halp::xyz_type<float>> value;

  int selectedNode{-1};
  bool dragging{false};
  bool executing{false};

  bool voronoiMode{false};
  float globalRadius{0.1f};

  std::function<void()> on_input_changed;
  std::function<void()> update;

  halp::xy_type<float> cursor{0.5f, 0.5f};
  halp::xy_type<float> runtime_cursor{0.5f, 0.5f};

  std::vector<unsigned char> pixmap;
  std::vector<float> distances;
  std::vector<float> weights;
};

}
