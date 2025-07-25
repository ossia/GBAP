#pragma once

#include <halp/custom_widgets.hpp>
#include <halp/value_types.hpp>
#include <avnd/wrappers/controls.hpp>

#include <cmath>
#include <vector>

namespace spat
{

struct NodesWidget
{
  static constexpr double width() { return 400.; }
  static constexpr double height() { return 400.; }

  std::vector<halp::xyz_type<float>> nodes;
  int selectedNode{-1};
  bool dragging{false};

  // Runtime feedback data
  std::vector<halp::xyz_type<float>> runtimeNodes;
  halp::xy_type<float> runtimeInputPoint{0.5f, 0.5f};

  // Callback to notify when nodes change
  //std::function<void()> on_nodes_changed;
  std::function<void()> on_input_changed;
  std::function<void()> update;

  void paint(auto ctx)
  {
    // Draw background
    ctx.set_fill_color({20, 20, 20, 255});
    ctx.begin_path();
    ctx.draw_rect(0., 0., width(), height());
    ctx.fill();

    // Draw nodes
    int i = 0;
    for(const auto& [x01, y01, r01] : nodes)
    {
      const double centerX = x01 * width();
      const double centerY = y01 * height();
      const double radius = r01 * std::min(width(), height());

      // Draw node circle
      if (i == selectedNode)
      {
        ctx.set_stroke_color({255, 200, 100, 255});
        ctx.set_stroke_width(3.);
        ctx.set_fill_color({100, 80, 50, 100});
      }
      else
      {
        ctx.set_stroke_color({150, 150, 150, 255});
        ctx.set_stroke_width(1.);
        ctx.set_fill_color({60, 60, 60, 100});
      }

      ctx.begin_path();
      ctx.draw_circle(centerX, centerY, radius);
      ctx.fill();
      ctx.stroke();

      // Draw node number
      ctx.set_fill_color({255, 255, 255, 255});
      ctx.begin_path();
      ctx.draw_text(centerX - 10, centerY + 5, std::to_string(i));
      ctx.fill();

      i++;
    }

    {
      auto& runtimeWeights = runtimeNodes;

      ctx.set_stroke_color({150, 150, 150, 5});
      ctx.set_stroke_width(1.);
      ctx.set_fill_color({60, 60, 60, 10});
      for(const auto& node : runtimeWeights)
      {
        const double centerX = node.x * width();
        const double centerY = node.y * height();
        const double radius = node.z * std::min(width(), height());

        // Draw weight as transparent overlay
        ctx.begin_path();
        ctx.draw_circle(centerX, centerY, radius);
        ctx.fill();
        ctx.stroke();
      }
    }

    // Draw input point (from value)
    ctx.set_fill_color({100, 200, 255, 255});
    ctx.begin_path();
    ctx.draw_circle(input_value.x * width(), input_value.y * height(), 5);
    ctx.fill();

    // Draw runtime input point if different
    if(std::abs(runtimeInputPoint.x - input_value.x) > 0.01f
       || std::abs(runtimeInputPoint.y - input_value.y) > 0.01f)
    {
      ctx.set_stroke_color({255, 100, 100, 255});
      ctx.set_stroke_width(2.);
      ctx.set_fill_color({255, 100, 100, 128});
      ctx.begin_path();
      ctx.draw_circle(runtimeInputPoint.x * width(), runtimeInputPoint.y * height(), 7);
      ctx.fill();
      ctx.stroke();
    }
  }

  // REFACTORME
  std::vector<float> computeWeights() const
  {
    std::vector<float> weights;
    weights.reserve(nodes.size());

    float totalWeight = 0.0f;

    for(const auto& [nx, ny, nr] : nodes)
    {
      float dx = input_value.x - nx;
      float dy = input_value.y - ny;
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
    double inputX = input_value.x * width();
    double inputY = input_value.y * height();
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
      input_value
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

  halp::transaction<std::vector<halp::xyz_type<float>>> transaction;
  std::vector<halp::xyz_type<float>> value;
  // halp::transaction<halp::xy_type<float>> transaction;
  halp::xy_type<float> input_value{0.5f, 0.5f};
};

}
