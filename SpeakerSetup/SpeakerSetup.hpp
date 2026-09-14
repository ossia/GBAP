#pragma once

#include "generic_model.hpp"

#include <ossia/detail/variant.hpp>

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>
#include <halp/sample_accurate_controls.hpp>

namespace spat
{

class SpeakerSetup
{
public:
  halp_meta(name, "Speaker Data")
  halp_meta(category, "Spatialization")
  halp_meta(c_name, "Nodes")
  halp_meta(uuid, "c610f7b6-cc41-4a2f-a68f-43bbb1b7cded")
  halp_meta(author, "ossia score")
  halp_meta(description, "Load speaker positions from various data files")
  halp_meta(manual_url, "https://ossia.io/score-docs/processes/speaker-data.html")

  enum OutputMode
  {
    Direct,
    OnlyPositions
  };
  struct in_t
  {
    struct obj_t : halp::file_port<"Speaker setup">
    {
      halp_meta(
          extensions,
          "All Supported (*.xld *.ease *.json *.rtf *.csv *.xml *.ioconfig);;EASE Files "
          "(*.xld "
          "*.ease);;AIIRA Files (*.json);;SPAT Files (*.rtf);;CSV Files "
          "(*.csv);;SpeakerView Files (*.xml);;4D Sound Files (*.xml);;Spat Revolution "
          "Files (*.ioconfig);;All Files (*)");
      static std::function<void(SpeakerSetup&)> process(file_type data);
    } obj;

    struct : halp::enum_t<OutputMode, "Output mode">
    {
      void update(SpeakerSetup& self) { self.update_outputs(); }
    } output_mode;
  } inputs;

  struct
  {
    halp::val_port<
        "Speakers",
        ossia::variant<
            boost::container::static_vector<spatparse::unified::loudspeaker, 256>,
            boost::container::static_vector<std::array<float, 3>, 256>>>
        speakers;
  } outputs;

  void update_outputs();

  std::vector<spatparse::unified::loudspeaker> m_last_speakers;
};
}
