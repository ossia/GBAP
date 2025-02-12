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

  int columns{4};
  int rows{4};

  halp::xy_type<float> sinkSize{0.1, 0.1};
  halp::xy_type<float> cursorSize{0.1, 0.1};
  halp::xy_type<float> intervSize{0.2, 0.2};

  void paint(auto ctx)
  {
    //Draw the background
    ctx.set_fill_color({10, 10, 10, 255});
    ctx.begin_path();
    ctx.draw_rect(0., 0., width(), height());
    ctx.fill();

    intervSize.x =  intervSize.x > 0 ? intervSize.x : 0;
    intervSize.y =  intervSize.y > 0 ? intervSize.y : 0;

    // Draw the grid
    if(columns > 0 && rows > 0)
    {
      ctx.set_fill_color({60, 60, 60, 255});
      ctx.begin_path();

      const auto rect_width = columns==1 ? width(): sinkSize.x * width();
      const auto rect_height = rows==1? height(): sinkSize.y * height();
      for(int x = 0; x < columns; x++)
      {
        for(int y = 0; y < rows; y++)
        {
          const auto xpos = x * (rect_width  + intervSize.x * width());
          const auto ypos = y * (rect_height + intervSize.y * height());
          ctx.draw_rect(xpos, ypos, rect_width, rect_height);
        }
      }
      ctx.fill();
    }

    // Draw the cursor
    ctx.begin_path();
    ctx.set_fill_color({0, 0, 255, 255});
    // Adjust position of the cursor to be centered on the x and y coordinates
    ctx.draw_rect((value.x - cursorSize.x / 2) * width(), (value.y - cursorSize.y / 2) * height() , cursorSize.x * width(), cursorSize.y * height());
    ctx.fill();
  }


  void setSinkSize(halp::xy_type<float> size) // Change the type to xy_type<float>
  {
    sinkSize = size;
    calcInterv();
  }
  void setCursorSize(halp::xy_type<float> size) // Change the type to xy_type<float>
  {
    cursorSize = size;
    calcInterv();
  }

  void calcInterv()
  {
    intervSize.x = (columns-1) ?
                   (1-sinkSize.x)/(columns-1)-sinkSize.x : 0;
    intervSize.y = (rows-1) ?
                   (1-sinkSize.y)/(rows-1)-sinkSize.y : 0;
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
