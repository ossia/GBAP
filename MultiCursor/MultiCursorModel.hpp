#pragma once

#include <Engine/Node/CommonWidgets.hpp>

#include <ossia/network/value/value.hpp>

#include <halp/audio.hpp>
#include <halp/controls.hpp>
#include <halp/meta.hpp>

namespace spat
{

class MultiCursorManager
{
public:
  halp_meta(name, "Multi-Cursor Manager")
  halp_meta(c_name, "multicursormanager")
  halp_meta(category, "Spatialization")
  halp_meta(description, "Can generate a position table")
  halp_meta(manual_url, "https://ossia.io/score-docs/processes/multicursor.html")
  halp_meta(author, "Ahmed El Moudden, Société des Arts Technologiques")
  halp_meta(uuid, "20f771a1-0e9a-4db2-bb7c-011467d84ded")

  struct ins
  {
    struct
    {
      halp_meta(name, "Position")
      struct range {
        float min = 0;
        float max = 1;
        float init = 0.5;
      };
      enum widget { multi_slider_xy };
      std::vector<ossia::value> value;
    } pos;
  } inputs;

  struct
  {
    halp::val_port<"out", std::vector<ossia::value>> out;
  } outputs;

  using tick = halp::tick;

  void operator()(halp::tick t);

};

}
