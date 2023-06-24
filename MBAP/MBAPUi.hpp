#pragma once
#include <MBAP/GridWidget.hpp>
#include <MBAP/MBAPModel.hpp>
#include <halp/layout.hpp>
namespace Example
{

struct MBAP::ui
{
  using enum halp::colors;
  using enum halp::layouts;

  halp_meta(name, "Main")
  halp_meta(layout, hbox)
  halp_meta(background, dark)

  struct
  {
    halp_meta(layout, vbox)
    halp::item<&ins::gain> gain;
    halp::item<&ins::rolloff> rolloff;
    halp::item<&ins::normalize> normalize;
    halp::control<&ins::nSinksX> sx;
    halp::control<&ins::nSinksY> sy;
    halp::item<&ins::systemNumber> snum;
    halp::item<&ins::sinkSize> sinkSize;
    halp::item<&ins::cursorSize> cursorSize;
  } controls;

  halp::custom_control<mbap::GridWidget, &ins::pos> area;

  void on_control_update()
  {
    this->area.columns = controls.sx.value;
    this->area.rows = controls.sy.value;
  }
};
}
