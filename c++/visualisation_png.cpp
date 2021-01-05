#include "visualisation_png.h"

GraphPNG::GraphPNG(int _width, int _height, float window_left, float window_right, float window_bottom, float window_top) : img(_width, _height), width(_width), height(_height)
{
  assert(width > 100 && "min. resolution");
  assert(height > 100 && "min. resolution");
  assert(1 < window_right - window_left && "min. window size");
  assert(1 < window_top - window_bottom && "min. window size");
  assert(window_left < 0);
  assert(window_bottom < 0);
  assert(window_top > 0);
  assert(window_right > 0);

  ratio_x = (float)width / (window_right - window_left);
  ratio_y = (float)height / (window_top - window_bottom);
  shift_x = -window_left * ratio_x;
  shift_y = -window_bottom * ratio_y;
}

GraphPNG::GraphPNG(int _width, int _height, float window_left, float window_right, float window_bottom) : img(_width, _height), width(_width), height(_height)
{
  float window_top = (float)(height * (window_right - window_left)) / (float)width + window_bottom;

  assert(width > 100 && "min. resolution");
  assert(height > 100 && "min. resolution");
  assert(1 < window_right - window_left && "min. window size");
  assert(1 < window_top - window_bottom && "min. window size");
  assert(window_left < 0);
  assert(window_bottom < 0);
  assert(window_top > 0);
  assert(window_right > 0);

  ratio_x = (float)width / (window_right - window_left);
  ratio_y = (float)height / (window_top - window_bottom);
  shift_x = -window_left * ratio_x;
  shift_y = -window_bottom * ratio_y;
}

GraphPNG::GraphPNG(int _width, int _height, float window_left, float window_right) : img(_width, _height), width(_width), height(_height)
{
  float window_top = (float)(height * (window_right - window_left)) / (float)(2 * width);

  assert(width > 100 && "min. resolution");
  assert(height > 100 && "min. resolution");
  assert(1 < window_right - window_left && "min. window size");
  assert(1 < 2 * window_top && "min. window size");
  assert(window_left < 0);
  assert(window_top > 0);
  assert(window_right > 0);

  ratio_x = (float)width / (window_right - window_left);
  ratio_y = ratio_x;
  shift_x = -window_left * ratio_x;
  shift_y = (float)height / 2;
}

bool GraphPNG::plot(const png::rgb_pixel_16 &col, int x, int y, const float brightness)
{
  if (x > 0 && y > 0 && x < width && y < height)
  {
    png::rgb_pixel_16 old = img.get_pixel(x, y);
    uint32_t r = old.red + col.red * brightness;
    if (r > UINT16_MAX)
      r = (uint16_t)(UINT16_MAX);
    old.red = (uint16_t)r;
    uint32_t g = old.green + col.green * brightness;
    if (g > UINT16_MAX)
      g = (uint16_t)(UINT16_MAX);
    old.green = (uint16_t)g;
    uint32_t b = old.blue + col.blue * brightness;
    if (b > UINT16_MAX)
      b = (uint16_t)(UINT16_MAX);
    old.blue = (uint16_t)b;
    img.set_pixel(x, y, old);
    return true;
  }
  return false;
}

void GraphPNG::WuDrawLine(const png::rgb_pixel_16 &col, float x0, float y0, float x1, float y1)
{
  x0 += shift_x;
  x1 += shift_x;
  y0 = height - (y0 + shift_y);
  y1 = height - (y1 + shift_y);

  auto ipart = [](float x) -> int { return int(std::floor(x)); };
  auto round = [](float x) -> float { return std::round(x); };
  auto fpart = [](float x) -> float { return x - std::floor(x); };
  auto rfpart = [=](float x) -> float { return 1 - fpart(x); };

  const bool steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep)
  {
    std::swap(x0, y0);
    std::swap(x1, y1);
  }
  if (x0 > x1)
  {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  const float dx = x1 - x0;
  const float dy = y1 - y0;
  const float gradient = (dx == 0) ? 1 : dy / dx;

  int xpx11;
  float intery;
  {
    const float xend = round(x0);
    const float yend = y0 + gradient * (xend - x0);
    const float xgap = rfpart(x0 + 0.5f);
    xpx11 = int(xend);
    const int ypx11 = ipart(yend);
    if (steep)
    {
      plot(col, ypx11, xpx11, rfpart(yend) * xgap);
      plot(col, ypx11 + 1, xpx11, fpart(yend) * xgap);
    }
    else
    {
      plot(col, xpx11, ypx11, rfpart(yend) * xgap);
      plot(col, xpx11, ypx11 + 1, fpart(yend) * xgap);
    }
    intery = yend + gradient;
  }

  int xpx12;
  {
    const float xend = round(x1);
    const float yend = y1 + gradient * (xend - x1);
    const float xgap = rfpart(x1 + 0.5f);
    xpx12 = int(xend);
    const int ypx12 = ipart(yend);
    if (steep)
    {
      plot(col, ypx12, xpx12, rfpart(yend) * xgap);
      plot(col, ypx12 + 1, xpx12, fpart(yend) * xgap);
    }
    else
    {
      plot(col, xpx12, ypx12, rfpart(yend) * xgap);
      plot(col, xpx12, ypx12 + 1, fpart(yend) * xgap);
    }
  }

  if (steep)
    for (int x = xpx11 + 1; x < xpx12; x++)
    {
      plot(col, ipart(intery), x, rfpart(intery));
      plot(col, ipart(intery) + 1, x, fpart(intery));
      intery += gradient;
    }
  else
    for (int x = xpx11 + 1; x < xpx12; x++)
    {
      plot(col, x, ipart(intery), rfpart(intery));
      plot(col, x, ipart(intery) + 1, fpart(intery));
      intery += gradient;
    }
}

void GraphPNG::drawAxis(const png::rgb_pixel_16 &col)
{
  int dist_seperation_lines_x = ratio_x;
  int dist_seperation_lines_y = ratio_y;

  WuDrawLine(col, 0, -shift_y, 0, height - shift_y);
  WuDrawLine(col, -shift_x, 0, width - shift_x, 0);

  for (int i = dist_seperation_lines_y; i < height - shift_y; i += dist_seperation_lines_y)
    WuDrawLine(col, -len_seperation_lines, i, len_seperation_lines, i);
  for (int i = dist_seperation_lines_y; i >= -shift_y; i -= dist_seperation_lines_y)
    WuDrawLine(col, -len_seperation_lines, i, len_seperation_lines, i);
  for (int i = dist_seperation_lines_x; i < width - shift_x; i += dist_seperation_lines_x)
    WuDrawLine(col, i, -len_seperation_lines, i, len_seperation_lines);
  for (int i = dist_seperation_lines_x; i >= -shift_x; i -= dist_seperation_lines_x)
    WuDrawLine(col, i, -len_seperation_lines, i, len_seperation_lines);
}

void GraphPNG::drawFunction(const NodePtr &f, int varId, vector<decimal> variables, const png::rgb_pixel_16 &col)
{
  variables[varId] = -shift_x / ratio_x;
  float r2, r1 = ratio_y * (float)eval(f, variables);
  for (int x = -shift_x + 1; x < width - shift_x; ++x)
  {
    variables[varId] = x / ratio_x;
    r2 = ratio_y * (float)eval(f, variables);
    WuDrawLine(col, (float)x, r1, (float)(x + 1), r2);
    r1 = r2;
  }
}

void GraphPNG::save(string name)
{
  img.write(name);
}