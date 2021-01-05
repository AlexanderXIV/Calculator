#pragma once
#include "defs.h"
#include "parser.h"

#include <math.h>
#include <string>
#include <vector>
#include <assert.h>

#include <png++/image.hpp>
#include <png++/rgb_pixel.hpp>

using std::abs;
using std::max;
using std::min;
using std::string;
using std::swap;
using std::vector;

#define len_seperation_lines 5

#define MAX_VAL (uint16_t)(UINT16_MAX)
#define RED png::rgb_pixel_16(MAX_VAL, 0, 0)
#define GREEN png::rgb_pixel_16(0, MAX_VAL, 0)
#define BLUE png::rgb_pixel_16(0, 0, MAX_VAL)
#define YELLOW png::rgb_pixel_16(MAX_VAL, MAX_VAL, 0)
#define CYAN png::rgb_pixel_16(0, MAX_VAL, MAX_VAL)
#define MAGENTA png::rgb_pixel_16(MAX_VAL, 0, MAX_VAL)
#define WHITE png::rgb_pixel_16(MAX_VAL, MAX_VAL, MAX_VAL)

struct GraphPNG
{
  png::image<png::rgb_pixel_16> img;
  int width, height, shift_x, shift_y;
  float ratio_x, ratio_y;

  GraphPNG(int _width, int _height, float window_left, float window_right, float window_bottom, float window_top);

  GraphPNG(int _width, int _height, float window_left, float window_right, float window_bottom);

  GraphPNG(int _width, int _height, float window_left, float window_right);

  bool plot(const png::rgb_pixel_16 &col, int x, int y, const float brightness);

  void WuDrawLine(const png::rgb_pixel_16 &col, float x0, float y0, float x1, float y1);

  void drawAxis(const png::rgb_pixel_16 &col = WHITE);

  void drawFunction(const NodePtr &f, int varId, vector<decimal> variables, const png::rgb_pixel_16 &col = WHITE);

  void save(string name);
};