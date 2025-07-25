#pragma once

#include "NodesModel.hpp"
#include "NodesWidget.hpp"

#include <halp/layout.hpp>

namespace spat
{

struct Nodes::ui
{
  using enum halp::colors;
  using enum halp::layouts;

  halp_meta(name, "Main")
  halp_meta(layout, vbox)
  halp_meta(background, dark)

  struct
  {
    halp_meta(layout, hbox)
    halp_meta(background, mid)
    struct : halp::control<&ins::inputPoint>
    {
      std::function<void(halp::xy_type<float>)> set;
    } inputPoint;
    halp::item<&ins::globalRadius> globalRadius;
    halp::item<&ins::normalize> normalize;
  } controls;

  halp::custom_control<NodesWidget, &ins::nodes> nodesArea;

  struct bus
  {
    void init(ui& ui)
    {
      // Set up callback when nodes change in the widget
      // ui.nodesArea.on_nodes_changed = [&ui, this] {
      //   // Send message to processing thread
      //   this->send_message(Nodes::nodes_update_message{.nodes = ui.nodesArea.nodes});
      // };
      ui.nodesArea.on_input_changed = [&ui, this] {
        // Send message to processing thread
        // FIXME support an update method here?
        ui.controls.inputPoint.set(ui.nodesArea.input_value);
        this->send_message(Nodes::nodes_update_message{.nodes = ui.nodesArea.nodes});
      };
    }

    // Process messages from the processing thread
    static void do_process(ui& self, Nodes::nodes_to_ui_message msg)
    {
      self.nodesArea.nodes = std::move(msg.nodes);
    }

    static void do_process(ui& self, Nodes::execution_value_to_ui msg)
    {
      self.nodesArea.runtimeNodes = std::move(msg.nodes);
      self.nodesArea.runtimeInputPoint = msg.inputPoint;
      self.nodesArea.update();
    }

    static void process_message(
        ui& self,
        std::variant<Nodes::nodes_to_ui_message, Nodes::execution_value_to_ui> msg)
    {
      std::visit([&self](auto& m) { do_process(self, m); }, msg);
    }

    std::function<void(Nodes::nodes_update_message)> send_message;
  };
};

}
