#pragma once

#include "NodesModel.hpp"
#include "NodesUi.hpp"

#if 0
#pragma once

#include <ossia/network/value/value.hpp>

#include <halp/controls.hpp>
#include <halp/custom_widgets.hpp>
#include <halp/layout.hpp>
#include <halp/meta.hpp>
#include <halp/sample_accurate_controls.hpp>
#include <halp/value_types.hpp>

#include <cmath>

#include <vector>

namespace spat
{

struct Node
{
  halp::xy_type<float> position{0.5f, 0.5f};
  float radius{0.1f};
};

struct NodesWidget
{
  static constexpr double width() { return 400.; }
  static constexpr double height() { return 400.; }

  std::vector<Node> nodes;
  halp::xy_type<float> inputPoint{0.5f, 0.5f};
  int selectedNode{-1};
  bool editingRadius{false};

  void paint(auto ctx)
  {
    // Draw background
    ctx.set_fill_color({20, 20, 20, 255});
    ctx.begin_path();
    ctx.draw_rect(0., 0., width(), height());
    ctx.fill();

    // Draw nodes
    int i = 0;
    for (const auto& node : nodes)
    {
      const double centerX = node.position.x * width();
      const double centerY = node.position.y * height();
      const double radius = node.radius * std::min(width(), height());

      // Draw node circle
      if (i == selectedNode && editingRadius)
      {
        ctx.set_stroke_color({255, 100, 100, 255});
        ctx.set_stroke_width(3.);
        ctx.set_fill_color({100, 50, 50, 100});
      }
      else if (i == selectedNode)
      {
        ctx.set_stroke_color({200, 200, 200, 255});
        ctx.set_stroke_width(2.);
        ctx.set_fill_color({80, 80, 80, 100});
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

    // Draw input point
    ctx.set_fill_color({100, 200, 255, 255});
    ctx.begin_path();
    ctx.draw_circle(inputPoint.x * width(), inputPoint.y * height(), 5);
    ctx.fill();
  }

  float distance(const halp::xy_type<float>& p1, const halp::xy_type<float>& p2) const
  {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return std::sqrt(dx * dx + dy * dy);
  }

  int findNodeAt(double x, double y) const
  {
    halp::xy_type<float> pos{
        static_cast<float>(x / width()),
        static_cast<float>(y / height())
    };

    for (int i = 0; i < std::ssize(nodes); ++i)
    {
      if (distance(pos, nodes[i].position) < 0.03f)
      {
        return i;
      }
    }
    return -1;
  }

  bool mouse_press(double x, double y)
  {
    int nodeIndex = findNodeAt(x, y);

    if (nodeIndex >= 0)
    {
      selectedNode = nodeIndex;
      editingRadius = false; // For now, simple mode
    }
    else
    {
      // Create new node
      nodes.push_back(Node{
          halp::xy_type<float>{
              static_cast<float>(x / width()),
              static_cast<float>(y / height())
          },
          0.1f
      });
      selectedNode = std::ssize(nodes) - 1;
    }

    update();
    return true;
  }

  void mouse_move(double x, double y)
  {
    if (selectedNode >= 0 && selectedNode < std::ssize(nodes))
    {
      nodes[selectedNode].position = halp::xy_type<float>{
          static_cast<float>(std::clamp(x / width(), 0.0, 1.0)),
          static_cast<float>(std::clamp(y / height(), 0.0, 1.0))
      };
      update();
    }
  }

  void mouse_release(double x, double y)
  {
    selectedNode = -1;
    update();
  }

  bool mouse_double_click(double x, double y)
  {
    // Delete node on double click
    int nodeIndex = findNodeAt(x, y);
    if (nodeIndex >= 0)
    {
      nodes.erase(nodes.begin() + nodeIndex);
      selectedNode = -1;
      update();
    }
    return true;
  }

  void wheel(double x, double y, double delta)
  {
    int nodeIndex = findNodeAt(x, y);
    if (nodeIndex >= 0)
    {
      // Adjust radius with wheel
      nodes[nodeIndex].radius += delta * 0.001f;
      nodes[nodeIndex].radius = std::clamp(nodes[nodeIndex].radius, 0.01f, 0.5f);
      update();
    }
  }

  std::function<void()> update;
};

class Nodes
{
public:
  halp_meta(name, "Nodes")
  halp_meta(category, "Control/Spatialization")
  halp_meta(c_name, "Nodes")
  halp_meta(uuid, "9e7f5d2a-b3c4-4e8a-9f1d-6a2b3c4d5e6f")
  halp_meta(author, "ossia score")
  halp_meta(description, "Node-based distance weighting for preset interpolation")
  halp_meta(manual_url, "https://ossia.io/score-docs/processes/nodes.html")

  struct ins
  {
    halp::xy_spinboxes_f32<"Input Point", halp::range{.min = 0., .max = 1., .init = 0.5}> inputPoint;
    halp::val_port<"Nodes", std::vector<ossia::value>> nodes;
  } inputs;

  struct outs
  {
    halp::val_port<"Weights", std::vector<ossia::value>> weights;
  } outputs;

  void operator()()
  {
    // Compute weights based on input point and nodes
    outputs.weights.value.clear();

    if (inputs.nodes.value.empty())
      return;

    float totalWeight = 0.0f;
    std::vector<float> weights;
    weights.reserve(inputs.nodes.value.size());

    // Calculate weights for each node
    for (const auto& node_val : inputs.nodes.value)
    {
      auto vec = ossia::convert<ossia::vec3f>(node_val);
      halp::xy_type<float> nodePos{vec[0], vec[1]};
      float nodeRadius = vec[2];

      float dx = inputs.inputPoint.value.x - nodePos.x;
      float dy = inputs.inputPoint.value.y - nodePos.y;
      float dist = std::sqrt(dx * dx + dy * dy);

      float weight = 0.0f;
      if (dist < nodeRadius)
      {
        // Linear falloff within radius
        weight = 1.0f - (dist / nodeRadius);
        weight = std::max(0.0f, std::min(1.0f, weight));
      }

      weights.push_back(weight);
      totalWeight += weight;
    }

    // Normalize weights if total > 1
    if (totalWeight > 1.0f && totalWeight > 0.0f)
    {
      for (auto& w : weights)
      {
        w /= totalWeight;
      }
    }

    // Convert to ossia values
    for (auto w : weights)
    {
      outputs.weights.value.push_back(ossia::value{w});
    }
  }

  struct ui
  {
    halp_meta(name, "Main")
    halp_meta(layout, halp::layouts::vbox)
    halp_meta(background, halp::colors::dark)

    halp::control<&ins::inputPoint> inputControl;
    halp::custom_actions_item<NodesWidget> nodesDisplay;

    struct bus
    {
      struct nodes_message
      {
        halp_flag(relocatable);
        std::vector<Node> nodes;
      };

      struct input_message
      {
        halp_flag(relocatable);
        halp::xy_type<float> point;
      };

      void init(ui& ui)
      {
        // Connect the input point control to the widget
        ui.nodesDisplay.inputPoint = ui.inputControl.value;

        // Set up update callback
        ui.nodesDisplay.update = [&ui, this] {
          // Send nodes data to processing thread
          std::vector<ossia::value> node_values;
          for (const auto& node : ui.nodesDisplay.nodes)
          {
            node_values.push_back(ossia::vec3f{node.position.x, node.position.y, node.radius});
          }
          
          // This would need proper message passing in real implementation
          // For now, we'll just update the display
        };
      }

      static void process_message(ui& self, nodes_message&& msg)
      {
        self.nodesDisplay.nodes = std::move(msg.nodes);
        if (self.nodesDisplay.update)
          self.nodesDisplay.update();
      }

      static void process_message(ui& self, input_message&& msg)
      {
        self.nodesDisplay.inputPoint = msg.point;
        if (self.nodesDisplay.update)
          self.nodesDisplay.update();
      }
    };
  };
};

}

#endif
