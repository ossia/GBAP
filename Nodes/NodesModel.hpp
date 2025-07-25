#pragma once

#include <ossia/network/value/value.hpp>

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <halp/sample_accurate_controls.hpp>

#include <cmath>
#include <vector>

namespace spat
{

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
    struct : halp::xy_spinboxes_f32<"Input Point", halp::range{.min = 0., .max = 1., .init = 0.5}>
    {
      void update(Nodes& self) { self.updateWeights(); }
    } inputPoint;

    struct : halp::val_port<"Nodes", std::vector<halp::xyz_type<float>>>
    {
      enum widget
      {
        control
      };
      void update(Nodes& self) { self.updateNodesFromInput(); }
    } nodes;
    
    halp::knob_f32<"Global Radius", halp::range{.min = 0.01, .max = 0.5, .init = 0.1}> globalRadius;
    halp::toggle<"Normalize"> normalize;
  } inputs;

  struct outs
  {
    halp::val_port<"Weights", std::vector<float>> weights;
  } outputs;

  using setup = halp::setup;
  void prepare(halp::setup info) {}

  using tick = halp::tick;
  void operator()(halp::tick t)
  {
    updateWeights();
  }

  struct ui;

  struct Node
  {
    float x{0.5f};
    float y{0.5f};
    float radius{0.1f};
  };

  std::vector<Node> m_nodes;

  struct nodes_update_message
  {
    halp_flag(relocatable);
    std::vector<halp::xyz_type<float>> nodes;
  };

  struct nodes_to_ui_message
  {
    halp_flag(relocatable);
    std::vector<halp::xyz_type<float>> nodes;
  };

  struct execution_value_to_ui
  {
    halp_flag(relocatable);
    std::vector<halp::xyz_type<float>> nodes;
    halp::xy_type<float> inputPoint;
  };

  // Process messages from UI
  void process_message(const nodes_update_message& msg)
  {
    inputs.nodes.value = msg.nodes;
    updateNodesFromInput();
  }

  // Send messages to UI
  std::function<void(
      std::variant<Nodes::nodes_update_message, Nodes::execution_value_to_ui>)>
      send_message;

  void updateNodesFromInput()
  {
    m_nodes.clear();
    for(const auto& v : inputs.nodes.value)
    {
      m_nodes.push_back(Node{v.x, v.y, v.z});
    }
    updateWeights();
  }

  void updateWeights()
  {
    outputs.weights.value.clear();

    if (m_nodes.empty())
      return;

    float totalWeight = 0.0f;
    std::vector<float> weights;
    weights.reserve(m_nodes.size());

    const float inputX = inputs.inputPoint.value.x;
    const float inputY = inputs.inputPoint.value.y;

    // Calculate weights for each node
    for (const auto& node : m_nodes)
    {
      float dx = inputX - node.x;
      float dy = inputY - node.y;
      float dist = std::sqrt(dx * dx + dy * dy);

      float weight = 0.0f;
      if (dist < node.radius)
      {
        // Linear falloff within radius
        weight = 1.0f - (dist / node.radius);
        weight = std::max(0.0f, std::min(1.0f, weight));
      }

      weights.push_back(weight);
      totalWeight += weight;
    }

    // Normalize weights if requested and total > 1
    if (inputs.normalize && totalWeight > 1.0f && totalWeight > 0.0f)
    {
      for (auto& w : weights)
      {
        w /= totalWeight;
      }
    }

    // Convert to ossia values
    for (auto w : weights)
    {
      outputs.weights.value.push_back(w);
    }

    // Send feedback to UI
    if (send_message)
    {
      send_message(
          execution_value_to_ui{.nodes = inputs.nodes, .inputPoint = inputs.inputPoint});
    }
  }

  ~Nodes() { send_message(execution_value_to_ui{}); }
};

}
