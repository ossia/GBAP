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
  halp_meta(background, background_dark)

  struct
  {
    halp_meta(layout, hbox)
    struct : halp::control<&ins::inputPoint>
    {
      std::function<void(halp::xy_type<float>)> set;
    } inputPoint;
    halp::control<&ins::smooth> globalRadius;
    halp::control<&ins::normalize> normalize;
    halp::control<&ins::voronoiMode> voronoiMode;
  } controls;

  halp::custom_control<NodesWidget, &ins::nodes> nodesArea;

  void start() { nodesArea.executing = true; }
  void stop()
  {
    nodesArea.reset();
    nodesArea.voronoiMode = controls.voronoiMode.value;
    nodesArea.scale = controls.globalRadius.value;
  }
  void reset() { stop(); }

  void on_control_update()
  {
    nodesArea.cursor = controls.inputPoint.value;
    nodesArea.nodes = nodesArea.value;
    nodesArea.voronoiMode = controls.voronoiMode.value;
    nodesArea.scale = controls.globalRadius.value;
  }
  struct bus
  {
    void init(ui& ui)
    {
      ui.nodesArea.voronoiMode = ui.controls.voronoiMode.value;
      ui.nodesArea.cursor = ui.controls.inputPoint.value;
      ui.nodesArea.nodes = ui.nodesArea.value;

      ui.nodesArea.on_input_changed = [&ui, this] {
        ui.controls.inputPoint.set(ui.nodesArea.cursor);
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
      self.nodesArea.runtime_cursor = msg.inputPoint;
      self.nodesArea.voronoiMode = msg.voronoiMode;
      self.nodesArea.globalRadius = msg.globalRadius;
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
