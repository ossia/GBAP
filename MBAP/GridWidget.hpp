#pragma once

#include <halp/custom_widgets.hpp>
#include <halp/value_types.hpp>

#include <avnd/wrappers/controls.hpp>

namespace mbap
{

struct GridWidget
{
  static constexpr double width() { return 400.; }
  static constexpr double height() { return 400.; }

  int columns{3};
  int rows{2};

  halp::xy_type<float> cursorSize{10, 10}; // Change cursorSize to xy_type<float>

  void paint(auto ctx)
  {
    // Draw the background
    // ctx.set_fill_color({120, 120, 120, 255});
    // ctx.begin_path();
    // ctx.draw_rect(0., 0., width(), height());
    // ctx.fill();

    // Draw the grid
    if(columns > 0 && rows > 0)
    {
      ctx.set_fill_color({60, 60, 60, 255});
      ctx.begin_path();
      const auto rect_width = 0.9 * width() / columns;
      const auto rect_height = 0.9 * height() / rows;
      for(int x = 0; x < columns; x++)
      {
        for(int y = 0; y < rows; y++)
        {
          const auto xpos = x * width() / columns;
          const auto ypos = y * height() / rows;
          ctx.draw_rect(xpos, ypos, rect_width, rect_height);
        }
      }
      ctx.fill();
    }

    // Draw the cursor
    ctx.begin_path();
    ctx.set_fill_color({90, 90, 90, 255});
    // Adjust position of the cursor to be centered on the x and y coordinates
    ctx.draw_rect(value.x * width() - cursorSize.x / 2, value.y * height() - cursorSize.y / 2, cursorSize.x, cursorSize.y);
    ctx.fill();
  }


  void setCursorSize(halp::xy_type<float> size) // Change the type to xy_type<float>
  {
    cursorSize = size;
  }


  bool mouse_press(double x, double y)
  {
    transaction.start();
    mouse_move(x, y);
    return true;
  }

  void mouse_move(double x, double y)
  {
    // const double res = std::clamp(x / width(), 0., 1.);
    transaction.update({(float)(x / width()), (float)(y / height())});
  }

  void mouse_release(double x, double y)
  {
    mouse_move(x, y);
    transaction.commit();
  }

  halp::transaction<halp::xy_type<float>> transaction;
  halp::xy_type<float> value{};
};
}
